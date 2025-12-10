#include "conn/server.hpp"

#include "utils/exception.hpp"
#include "utils/logger.hpp"
#include "utils/utils.hpp"
#include "core/network.hpp"
#include "conn/client.hpp"
#include <cstring>
#include <cerrno>
#include <arpa/inet.h>
#include <poll.h>

namespace conn
{
	ServerSocket::ServerSocket(const int port, const std::string &host, const config::ServerConfig &serverConfig) : Connection(-1, Connection::SERVER_SOCKET), _port(port), _host(host), _serverConfig(serverConfig)
	{
		if (_port <= 0 || _port > 65535)
			EXCEPTION("Invalid port number: %d", _port);

		std::memset(&_address, 0, sizeof(_address));
		_address.sin_family = AF_INET;
		_address.sin_port = htons(static_cast<in_port_t>(_port));
		if (_host.empty() || _host == "localhost")
			_address.sin_addr.s_addr = htonl(INADDR_ANY);
		else
		{
			if (inet_pton(AF_INET, _host.c_str(), &_address.sin_addr) <= 0)
				EXCEPTION("Invalid host address: %s", _host.c_str());
		}

		setPollEvents(POLLIN | POLLHUP | POLLERR);

		const int socketFd = socket(AF_INET, SOCK_STREAM, 0);
		if (socketFd < 0)
			EXCEPTION("Failed to create socket: %s", std::strerror(errno));
		setFd(socketFd);

		int opt = 1;
		if (setsockopt(getFd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			EXCEPTION("Failed to set socket options: %s", std::strerror(errno));

		if (bind(getFd(), reinterpret_cast<sockaddr *>(&_address), sizeof(_address)) < 0)
			EXCEPTION("Failed to bind socket to %s:%d: %s", _host.c_str(), _port, std::strerror(errno));

		if (listen(getFd(), SOMAXCONN) < 0)
			EXCEPTION("Failed to listen on socket: %s", std::strerror(errno));

		LOG_INFO("Server listening on %s:%d", _host.c_str(), _port);
	}

	ServerSocket::~ServerSocket()
	{
	}

	int ServerSocket::acceptNewClient()
	{
		sockaddr_in clientAddr;
		socklen_t clientAddrLen = sizeof(clientAddr);

		const int clientFd = accept(getFd(), reinterpret_cast<sockaddr *>(&clientAddr), &clientAddrLen);
		if (clientFd < 0)
		{
			if (errno != EWOULDBLOCK && errno != EAGAIN)
				EXCEPTION("Failed to accept new client: %s", std::strerror(errno));
			else
				LOG_WARNING("No pending connections to accept");
			return -1;
		}

		char clientIp[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientAddr.sin_addr, clientIp, sizeof(clientIp));
		LOG_INFO("Accepted new client connection from %s:%d, fd: %d", clientIp, ntohs(clientAddr.sin_port), clientFd);

		return clientFd;
	}

	void ServerSocket::handlePollIn()
	{
		const int clientFd = acceptNewClient();
		if (clientFd < 0)
			EXCEPTION("Failed to accept new client connection on server socket fd: %d", getFd());

		core::Network::getInstance()->registerConnection(new ClientSocket(clientFd, _serverConfig));
	}

	void ServerSocket::handlePollOut()
	{
		LOG_DEBUG("ServerSocket fd: %d handlePollOut called", getFd());
	}

	void ServerSocket::handlePollErr()
	{
		LOG_DEBUG("ServerSocket fd: %d handlePollErr called", getFd());
	}

	void ServerSocket::handleEvents(const short events)
	{
		// LOG_DEBUG("ServerSocket fd: %d handleEvents called with events: %s", getFd(), utils::getEventNames(events).c_str());
		if (events & POLLIN)
			handlePollIn();
		if (events & POLLOUT)
			handlePollOut();
		if (events & (POLLERR | POLLHUP | POLLNVAL))
		{
			LOG_WARNING("ServerSocket fd: %d received hang up or error event", getFd());
			handlePollErr();
		}
	}
} // namespace conn
