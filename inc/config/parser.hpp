#pragma once

#include <string>
#include "config/server.hpp"
#include "config/location.hpp"
#include "config/constant.hpp"

namespace config
{
	class ParserConfig
	{
	private:
		std::string _configFilePath;
		std::vector<ServerConfig> _servers;

		bool cleanUpAndContinue(std::string &line);
		void parseLocationBlock(std::ifstream &file, LocationConfig &locationConfig);
		void parseServerBlock(std::ifstream &file, ServerConfig &serverConfig);

	public:
		ParserConfig(const std::string &configFilePath);
		~ParserConfig();

		void parseConfigFile();

		void setup();
	};
} // namespace config
