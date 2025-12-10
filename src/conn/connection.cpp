#include "conn/connection.hpp"

#include "utils/logger.hpp"
#include <unistd.h>

namespace conn
{
	Connection::Connection(const int fd, const ConnectionType type) : _fd(fd), _type(type), _pollEvents(0), _lastActivity(std::time(NULL))
	{
	}

	Connection::~Connection()
	{
		if (_fd >= 0)
		{
			if (close(_fd) < 0)
				LOG_WARNING("Failed to close file descriptor: %d", _fd);
		}
	}

	bool Connection::isTimedOut(const int timeoutSeconds) const
	{
		if (_lastActivity == 0)
			return false;

		time_t currentTime = std::time(NULL);
		return (std::difftime(currentTime, _lastActivity) >= timeoutSeconds);
	}
} // namespace conn
