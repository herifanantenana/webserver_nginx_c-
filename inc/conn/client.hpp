#pragma once

#include "conn/connection.hpp"
#include "config/server.hpp"

namespace conn
{
	class ClientSocket : public Connection
	{
	public:
		enum ClientState
		{
			READING_REQUEST,
			PROCESSING_REQUEST,
			EXECUTING_CGI,
			WRITING_RESPONSE,
			KEEP_ALIVE,
			CLOSING,
			CLIENT_ERROR
		};

	private:
		const config::ServerConfig &_serverConfig;
		ClientState _state;

	public:
		ClientSocket(const int fd, const config::ServerConfig &serverConfig);
		virtual ~ClientSocket();

		inline ClientState &getState() { return _state; }

		inline virtual bool shouldClose() const { return _state == CLOSING || _state == CLIENT_ERROR; };
	};
} // namespace conn
