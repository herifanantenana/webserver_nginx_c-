#pragma once

#include "config/server.hpp"
#include "conn/connection.hpp"
#include <vector>
#include <poll.h>

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
		std::map<const int, conn::Connection *> _connections;

		Network();
		~Network();

		void registerConnection(conn::Connection *connection);
		void unregisterConnection(conn::Connection *connection);

		void setupServerSocket();
		void synchronizePollFds();
		void cleanUpTimeOutClients();

	public:
		static Network *getInstance();
		static void destroyInstance();

		void init(std::vector<config::ServerConfig> serverConfigs);
		void run();
	};
} // namespace core
