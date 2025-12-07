#pragma once

#include <string>
#include "config/server.hpp"
#include "config/location.hpp"

namespace config
{
	class ParserConfig
	{
	private:
		std::string _configFilePath;
		std::vector<ServerConfig> _servers;

	public:
		ParserConfig(const std::string &configFilePath);
		~ParserConfig();
	};
} // namespace config
