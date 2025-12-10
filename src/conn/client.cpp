#include "conn/client.hpp"

#include "utils/logger.hpp"
#include "utils/utils.hpp"
#include <poll.h>

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
		// LOG_DEBUG("ClientSocket fd: %d handlePollIn called", getFd());
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
		// LOG_DEBUG("ClientSocket fd: %d handleEvents called with events: %s", getFd(), utils::getEventNames(events).c_str());
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
