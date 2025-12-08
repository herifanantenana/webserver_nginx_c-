#include "connection/client.hpp"

namespace connection
{
	ClientSocket::ClientSocket(const int fd, const config::ServerConfig &serverConfig) : Connection(fd, Connection::CLIENT_SOCKET), _serverConfig(serverConfig)
	{
	}

	ClientSocket::~ClientSocket()
	{
	}
} // namespace connection
