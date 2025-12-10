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
				LOG_WARNING("Setting an invalid fd: %d", _fd);
		}
	}
} // namespace conn
