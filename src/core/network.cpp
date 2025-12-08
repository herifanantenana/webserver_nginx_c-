#include "core/network.hpp"

#include <csignal>
#include "utils/logger.hpp"

namespace core
{
	Network *Network::_instance = NULL;
	bool Network::_isRunning = false;

	void Network::handleSignal(int signal)
	{
		if (signal == SIGINT || signal == SIGTERM)
		{
			LOG_INFO("Signal %d received, shutting down network...", signal);
			Network::_isRunning = false;
		}
	}

	Network::Network() : _serverConfigs(), _pollFds(), _connections()
	{
	}

	Network::~Network()
	{
		for (std::map<const int, connection::Connection *>::iterator it = _connections.begin(); it != _connections.end(); ++it)
			delete it->second;

		_serverConfigs.clear();
		_pollFds.clear();
		_connections.clear();
	}

	Network *Network::getInstance()
	{
		if (_instance == NULL)
			_instance = new Network();

		return _instance;
	}

	void Network::destroyInstance()
	{
		if (_instance != NULL)
		{
			delete _instance;
			_instance = NULL;
			Network::_isRunning = false;
		}
	}
} // namespace core
