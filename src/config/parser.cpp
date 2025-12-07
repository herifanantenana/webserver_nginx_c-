#include "config/parser.hpp"

namespace config
{
	ParserConfig::ParserConfig(const std::string &configFilePath) : _configFilePath(configFilePath), _servers()
	{
	}

	ParserConfig::~ParserConfig()
	{
	}
} // namespace config
