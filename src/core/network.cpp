#include "core/network.hpp"

#include "utils/logger.hpp"
#include "utils/exception.hpp"
#include "conn/server.hpp"
#include "conn/client.hpp"
#include "conn/connection.hpp"
#include <csignal>
#include <cstring>

namespace core
{
	Network *Network::_instance = NULL;
	bool Network::_isRunning = false;

	void Network::handleSignal(int signal)
	{
		if (signal == SIGINT || signal == SIGTERM)
		{
			LOG_FATAL("Signal %d received, shutting down network...", signal);
			Network::_isRunning = false;
		}
	}

	Network::Network() : _serverConfigs(), _pollFds(), _connections()
	{
	}

	Network::~Network()
	{
		for (std::map<const int, conn::Connection *>::iterator it = _connections.begin(); it != _connections.end(); ++it)
			delete it->second;

		_serverConfigs.clear();
		_pollFds.clear();
		_connections.clear();
	}

	Network *Network::getInstance()
	{
		if (_instance == NULL)
			_instance = new Network();

		return _instance;
	}

	void Network::destroyInstance()
	{
		if (_instance != NULL)
		{
			delete _instance;
			_instance = NULL;
			_isRunning = false;
		}
	}

	void Network::registerConnection(conn::Connection *connection)
	{
		const int fd = connection->getFd();
		if (fd < 0)
			EXCEPTION("Cannot register connection with invalid file descriptor %d.", fd);

		_connections.insert(std::make_pair(fd, connection));

		pollfd pfd;
		pfd.fd = fd;
		pfd.events = connection->getPollEvents();
		pfd.revents = 0;
		_pollFds.push_back(pfd);
	}

	void Network::unregisterConnection(conn::Connection *connection)
	{
		const int fd = connection->getFd();
		std::map<const int, conn::Connection *>::iterator connIt = _connections.find(fd);
		if (connIt != _connections.end())
		{
			delete connIt->second;
			_connections.erase(connIt);

			for (std::vector<pollfd>::iterator pfdIt = _pollFds.begin(); pfdIt != _pollFds.end(); ++pfdIt)
			{
				if (pfdIt->fd == fd)
				{
					_pollFds.erase(pfdIt);
					break;
				}
			}
		}
		else
			LOG_WARNING("Attempted to unregister non-existent connection with fd %d.", fd);
	}

	void Network::setupServerSocket()
	{
		LOG_DEBUG("Setting up server sockets...");
		for (std::vector<config::ServerConfig>::const_iterator it = _serverConfigs.begin(); it != _serverConfigs.end(); ++it)
		{
			std::vector<config::ServerConfig::HostPort> hostPorts = it->getHostPorts();
			if (hostPorts.empty())
				EXCEPTION("Server configuration has no ports defined.");

			for (std::vector<config::ServerConfig::HostPort>::const_iterator hpIt = hostPorts.begin(); hpIt != hostPorts.end(); ++hpIt)
			{
				conn::ServerSocket *serverSocket = NULL;
				try
				{
					serverSocket = new conn::ServerSocket(hpIt->second, hpIt->first, *it);
					registerConnection(serverSocket);
				}
				catch (const std::exception &e)
				{
					if (serverSocket)
						delete serverSocket;
					HANDLE_EXCEPTION(e);
				}
			}
		}

		if (_connections.empty())
			EXCEPTION("No server sockets were created. Check server configurations.");
	}

	void Network::init(std::vector<config::ServerConfig> serverConfigs)
	{
		LOG_DEBUG("Initializing network with server configurations...");
		if (serverConfigs.empty())
			EXCEPTION("No server configurations provided to initialize the network.");

		signal(SIGINT, handleSignal);
		signal(SIGTERM, handleSignal);

		_serverConfigs = serverConfigs;
	}

	void Network::synchronizePollFds()
	{
		std::vector<conn::Connection *> connectionsToClose;
		for (std::vector<pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it)
		{
			const int fd = it->fd;
			std::map<const int, conn::Connection *>::iterator connIt = _connections.find(fd);
			if (connIt == _connections.end())
			{
				LOG_WARNING("Pollfd with fd %d has no associated connection. Skipping.", fd);
				connectionsToClose.push_back(connIt->second);
				continue;
			}

			short events = POLLIN | POLLHUP | POLLERR;
			if (connIt->second->getType() == conn::Connection::CLIENT_SOCKET)
			{
				conn::ClientSocket *clientSocket = dynamic_cast<conn::ClientSocket *>(connIt->second);
				if (clientSocket)
				{
					conn::ClientSocket::ClientState clientState = clientSocket->getState();
					if (clientState == conn::ClientSocket::PROCESSING_REQUEST || clientState == conn::ClientSocket::EXECUTING_CGI || clientState == conn::ClientSocket::WRITING_RESPONSE)
						events |= POLLOUT;
					connIt->second->setPollEvents(events);
				}
				else
					LOG_WARNING("Pollfd with fd %d is not associated with a ClientSocket. Skipping.", fd);
			}

			it->events = connIt->second->getPollEvents();
		}

		for (std::vector<conn::Connection *>::iterator it = connectionsToClose.begin(); it != connectionsToClose.end(); ++it)
			unregisterConnection(*it);
	}

	void Network::cleanUpTimeOutClients()
	{
		std::vector<conn::Connection *> connectionsToClose;

		for (std::map<const int, conn::Connection *>::iterator it = _connections.begin(); it != _connections.end(); ++it)
		{
			conn::Connection *connection = it->second;
			if (connection->getType() == conn::Connection::SERVER_SOCKET)
				continue;

			// !fix: check for timeout prepare response for timed out clients
			if (connection->isTimedOut(TIMEOUT_CHECK_INTERVAL))
				connectionsToClose.push_back(connection);
		}

		// !fix: remove this code after implementing proper timeout response handling
		for (std::vector<conn::Connection *>::iterator it = connectionsToClose.begin(); it != connectionsToClose.end(); ++it)
		{
			conn::Connection *connection = *it;
			LOG_WARNING("Closing timed out client connection with fd %d.", connection->getFd());
			unregisterConnection(connection);
		}
	}

	void Network::handlePollEvents(int &eventCount)
	{
		std::vector<conn::Connection *> connectionsToClose;
		for (std::vector<pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end() && eventCount > 0; ++it)
		{
			const short revents = it->revents;
			if (revents == 0)
				continue;

			--eventCount;

			const int fd = it->fd;
			conn::Connection *connection = _connections[fd];
			if (!connection)
				continue;

			try
			{
				connection->handleEvents(revents);
				if (connection->shouldClose())
					connectionsToClose.push_back(connection);
			}
			catch (const std::exception &e)
			{
				// !fix:  prepare error response for client before closing
				HANDLE_EXCEPTION(e);
				connectionsToClose.push_back(connection);
			}

			// !fix: remove this code after implementing proper error response handling
			for (std::vector<conn::Connection *>::iterator closeIt = connectionsToClose.begin(); closeIt != connectionsToClose.end(); ++closeIt)
				unregisterConnection(*closeIt);
			connectionsToClose.clear();
		}
	}

	void Network::run()
	{
		LOG_INFO("Starting network event loop...");
		setupServerSocket();
		_isRunning = true;

		while (_isRunning)
		{
			synchronizePollFds();

			int eventCount = poll(&_pollFds[0], _pollFds.size(), 1000);
			if (eventCount < 0)
			{
				if (errno == EINTR)
					continue;
				EXCEPTION("Poll error: %s", std::strerror(errno));
			}

			if (eventCount > 0)
				handlePollEvents(eventCount);

			cleanUpTimeOutClients();
		}
	}
} // namespace core
