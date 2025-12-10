#include "config/location.hpp"

#include "utils/exception.hpp"
#include "utils/utils.hpp"
#include <iostream>

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

	void config::LocationConfig::printConfig() const
	{
		std::cout << "Location Config:" << std::endl;
		std::cout << "\tAlias Path: " << _aliasPath << std::endl;
		std::cout << "\tRoot Path: " << _rootPath << std::endl;
		std::cout << "\tAuto Index: " << (_autoIndex ? "Enabled" : "Disabled") << std::endl;

		std::cout << "\tIndex Files:" << std::endl;
		for (std::vector<std::string>::const_iterator it = _indexFiles.begin(); it != _indexFiles.end(); ++it)
			std::cout << "\t\t- " << *it << std::endl;

		std::cout << "\tAllowed Methods:" << std::endl;
		for (std::vector<std::string>::const_iterator it = _allowedMethods.begin(); it != _allowedMethods.end(); ++it)
			std::cout << "\t\t- " << *it << std::endl;

		std::cout << "\tCGI Mappings:" << std::endl;
		for (std::vector<CgiMapping>::const_iterator it = _cgiMappings.begin(); it != _cgiMappings.end(); ++it)
			std::cout << "\t\t- Extension: " << it->first << ", Script: " << it->second << std::endl;

		std::cout << "\tUpload Paths:" << std::endl;
		for (std::vector<std::string>::const_iterator it = _uploadPaths.begin(); it != _uploadPaths.end(); ++it)
			std::cout << "\t\t- " << *it << std::endl;

		if (!_redirect.second.empty())
			std::cout << "\tRedirect: " << _redirect.first << " -> " << _redirect.second << std::endl;
		else
			std::cout << "\tRedirect: None" << std::endl;
	}
} // namespace config
