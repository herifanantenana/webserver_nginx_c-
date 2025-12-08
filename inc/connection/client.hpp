#include "connection/connection.hpp"

#include "config/server.hpp"

namespace connection
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

		inline const ClientState &getState() const { return _state; }

		virtual bool shouldClose() const;
	};
} // namespace connection
