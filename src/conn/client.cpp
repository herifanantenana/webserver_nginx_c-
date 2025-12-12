#include "conn/client.hpp"

#include "utils/logger.hpp"
#include "utils/utils.hpp"
#include <poll.h>
#include <sys/socket.h>
#include <cstring>

namespace conn
{
	ClientSocket::ClientSocket(const int fd, const config::ServerConfig &serverConfig) : Connection(fd, Connection::CLIENT_SOCKET), _serverConfig(serverConfig), _state(READING_REQUEST)
	{
	}

	ClientSocket::~ClientSocket()
	{
	}

	void conn::ClientSocket::handlePollIn()
	{
		LOG_DEBUG("ClientSocket fd: %d handlePollIn called", getFd());
		updateLastActivity();
		LOG_FATAL("ClientSocket fd: %d updated last activity time", getFd());

		char buffer[1024];
		ssize_t bytesRead = recv(getFd(), buffer, sizeof(buffer) - 1, 0);
		if (bytesRead < 0)
		{
			LOG_ERROR("ClientSocket fd: %d recv error : %s", getFd(), std::strerror(errno));
			_state = CLIENT_ERROR;
			return;
		}
		if (bytesRead == 0)
		{
			LOG_INFO("ClientSocket fd: %d connection closed by peer", getFd());
			_state = CLOSING;
			return;
		}

		buffer[bytesRead] = '\0';
		LOG_CONSOLE("ClientSocket fd: %d received data: %s", getFd(), buffer);
	}

	void conn::ClientSocket::handlePollOut()
	{
		LOG_DEBUG("ClientSocket fd: %d handlePollOut called", getFd());
	}

	void conn::ClientSocket::handlePollErr()
	{
		LOG_DEBUG("ClientSocket fd: %d handlePollErr called", getFd());
	}

	void ClientSocket::handleEvents(const short events)
	{
		LOG_DEBUG("ClientSocket fd: %d handleEvents called with events: %s", getFd(), utils::getEventNames(events).c_str());
		if (events & POLLIN)
		{
			handlePollIn();
		}
		if (events & POLLOUT)
		{
			handlePollOut();
		}
		if (events & (POLLERR | POLLHUP | POLLNVAL))
		{
			LOG_WARNING("ClientSocket fd: %d received hang up or error event", getFd());
			handlePollErr();
		}
	}
} // namespace conn
