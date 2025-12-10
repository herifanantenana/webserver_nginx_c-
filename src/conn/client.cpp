#include "conn/client.hpp"

namespace conn
{
	ClientSocket::ClientSocket(const int fd, const config::ServerConfig &serverConfig) : Connection(fd, Connection::CLIENT_SOCKET), _serverConfig(serverConfig), _state(READING_REQUEST)
	{
	}

	ClientSocket::~ClientSocket()
	{
	}
} // namespace conn
