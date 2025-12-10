#include "core/network.hpp"

#include "utils/logger.hpp"
#include "utils/exception.hpp"
#include "conn/server.hpp"
#include <csignal>

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
} // namespace core
