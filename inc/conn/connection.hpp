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

		inline void setPollEvents(const short events) { _pollEvents = events; }
		inline void setFd(const int fd) { _fd = fd; }

		inline const int &getFd() const { return _fd; }
		inline const ConnectionType &getType() const { return _type; }
		inline const short &getPollEvents() const { return _pollEvents; }

		bool isTimeOuted(const int timeoutSeconds) const;

		virtual bool shouldClose() const = 0;
		virtual void handleEvents(const short events) = 0;
	};
} // namespace conn
