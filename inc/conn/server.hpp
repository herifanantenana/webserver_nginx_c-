#pragma once

#include "config/server.hpp"
#include "conn/connection.hpp"
#include <string>
#include <netinet/in.h>

namespace conn
{
	class ServerSocket : public Connection
	{
	private:
		const int _port;
		const std::string _host;
		const config::ServerConfig &_serverConfig;
		sockaddr_in _address;

	public:
		ServerSocket(const int port, const std::string &host, const config::ServerConfig &serverConfig);
		virtual ~ServerSocket();

		inline virtual bool shouldClose() const { return false; };
		virtual void handleEvents(const short events);
	};
} // namespace conn