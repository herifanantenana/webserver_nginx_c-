#include "config/location.hpp"

namespace config
{
	LocationConfig::LocationConfig() : _aliasPath(""), _rootPath(""), _autoIndex(false), _indexFiles(), _allowedMethods(), _cgiMappings(), _uploadPaths(), _redirect()
	{
	}

	LocationConfig::~LocationConfig()
	{
	}
} // namespace config
