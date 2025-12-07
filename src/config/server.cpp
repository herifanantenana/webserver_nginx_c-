#include "config/server.hpp"

namespace config
{
	ServerConfig::ServerConfig() : _hostPorts(), _rootPath(""), _errorPages(), _clientMaxBodySize(0), _locations()
	{
	}

	ServerConfig::~ServerConfig()
	{
	}
} // namespace config
