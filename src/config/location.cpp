#include "config/location.hpp"

#include "utils/exception.hpp"
#include "utils/utils.hpp"

namespace config
{
	LocationConfig::LocationConfig() : _aliasPath(""), _rootPath(""), _autoIndex(false), _indexFiles(), _allowedMethods(), _cgiMappings(), _uploadPaths(), _redirect()
	{
	}

	LocationConfig::~LocationConfig()
	{
	}

	void LocationConfig::setup(const std::string &serverRootPath)
	{
		if (_aliasPath.empty() || _aliasPath.at(0) != '/')
			EXCEPTION("Invalid URI path fork location: %s", _aliasPath.c_str());

		if (_rootPath.empty())
			_rootPath = serverRootPath;
		else if (!utils::startsWith(_rootPath, serverRootPath, "/"))
			EXCEPTION("Root path for location must be within server root: %s", _rootPath.c_str());

		for (std::vector<std::string>::iterator it = _indexFiles.begin(); it != _indexFiles.end(); ++it)
			*it = utils::buildPath(_rootPath, *it);

		for (std::vector<std::string>::iterator it = _uploadPaths.begin(); it != _uploadPaths.end(); ++it)
			*it = utils::buildPath(_rootPath, *it);

		if (!_redirect.second.empty() && (_redirect.first < 300 || _redirect.first > 399))
			EXCEPTION("Invalid redirect status code %d for location: %s", _redirect.first, _aliasPath.c_str());
	}
} // namespace config
