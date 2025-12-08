#include "connection/connection.hpp"

#include "config/server.hpp"

namespace connection
{
	class ClientSocket : public Connection
	{
	private:
		const config::ServerConfig &_serverConfig;

	public:
		ClientSocket(const int fd, const config::ServerConfig &serverConfig);
		virtual ~ClientSocket();
	};
} // namespace connection
