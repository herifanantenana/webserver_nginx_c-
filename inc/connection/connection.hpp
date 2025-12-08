#pragma once

#include <ctime>

namespace connection
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

		inline void setFd(const int fd) { _fd = fd; }
		inline void setPollEvents(const short pollEvents) { _pollEvents = pollEvents; }
		inline void updateActivity() { _lastActivity = std::time(NULL); }

		inline const int &getFd() const { return _fd; }
	};
} // namespace connection
