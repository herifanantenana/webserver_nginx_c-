#pragma once

#include <vector>
#include <poll.h>
#include "config/server.hpp"
#include "connection/connection.hpp"

namespace core
{
	class Network
	{
	private:
		static Network *_instance;
		static bool _isRunning;
		static void handleSignal(int signal);

		std::vector<config::ServerConfig> _serverConfigs;
		std::vector<pollfd> _pollFds;
		std::map<const int, connection::Connection *> _connections;

		Network();
		~Network();

	public:
		static Network *getInstance();
		static void destroyInstance();
	};
} // namespace network
