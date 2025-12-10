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

	void ClientSocket::handleEvents(const short events)
	{
		LOG_DEBUG("ClientSocket fd: %d handleEvents called with events: %s", getFd(), utils::getEventsName(events).c_str());
		if (events & POLLIN)
		{
		}
		if (events & POLLOUT)
		{
		}
		if (events & (POLLERR | POLLHUP | POLLNVAL))
		{
			LOG_WARNING("ClientSocket fd: %d received hang up or error event", getFd());
		}
	}
} // namespace conn
