#pragma once

#include <string>
#include <netinet/in.h>
#include "config/server.hpp"
#include "conn/connection.hpp"

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

		virtual bool shouldClose() const;
	};
} // namespace conn