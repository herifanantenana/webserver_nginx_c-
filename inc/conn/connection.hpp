#pragma once

#include <ctime>

namespace conn
{
	class Connection
	{
	public:
		enum ConnectionType
		{
			SERVER_SOCKET,
			CLIENT_SOCKET
		};

	private:
		int _fd;
		const ConnectionType _type;
		short _pollEvents;
		time_t _lastActivity;

	public:
		Connection(const int fd, const ConnectionType type);
		virtual ~Connection();
	};
} // namespace conn
