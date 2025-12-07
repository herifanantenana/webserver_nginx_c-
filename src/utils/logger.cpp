#include "utils/logger.hpp"
#include <iostream>
#include <cstdarg>
#include <cstdio>
#include <ctime>

namespace utils
{
	Logger::Logger()
	{
	}

	Logger::~Logger()
	{
	}

	const char *Logger::getLevelIcon(Level level)
	{
		switch (level)
		{
		case Logger::CONSOL:
			return "⚪";
		case Logger::DEBUG:
			return "🔵";
		case Logger::INFO:
			return "🟢";
		case Logger::WARNING:
			return "🟡";
		case Logger::ERROR:
			return "🔴";
		case Logger::FATAL:
			return "💀";
		default:
			return "⚪";
		}
	}

	const char *Logger::getLevelColor(Level level)
	{
		switch (level)
		{
		case Logger::CONSOL:
			return "\033[0m"; // Reset
		case Logger::DEBUG:
			return "\033[36m"; // Cyan
		case Logger::INFO:
			return "\033[32m"; // Green
		case Logger::WARNING:
			return "\033[33m"; // Yellow
		case Logger::ERROR:
			return "\033[31m"; // Red
		case Logger::FATAL:
			return "\033[35m"; // Magenta
		default:
			return "\033[0m"; // Reset
		}
	}

	const char *Logger::getLevelString(Level level)
	{
		switch (level)
		{
		case Logger::CONSOL:
			return "CONSOL";
		case Logger::DEBUG:
			return "DEBUG";
		case Logger::INFO:
			return "INFO";
		case Logger::WARNING:
			return "WARNING";
		case Logger::ERROR:
			return "ERROR";
		case Logger::FATAL:
			return "FATAL";
		default:
			return "UNKNOWN";
		}
	}

	std::string Logger::getCurrentTimestamp()
	{
		time_t now = std::time(NULL);
		struct tm *timeInfo = std::localtime(&now);

		char buffer[20];
		std::strftime(buffer, sizeof(buffer), "%H:%M:%S", timeInfo);
		return std::string(buffer);
	}

	void Logger::log(Level level, const char *file, const char *function, int line, const char *format, ...)
	{
		va_list args;
		va_start(args, format);

		char buffer[__INT16_MAX__];
		vsnprintf(buffer, sizeof(buffer), format, args);
		va_end(args);

		((level == Logger::ERROR || level == Logger::FATAL) ? std::cerr : std::cout) << getLevelIcon(level) << " " << "[" << getCurrentTimestamp() << "]" << " " << getLevelColor(level) << "[" << getLevelString(level) << "]" << getLevelColor(Logger::CONSOL) << " " << file << ":" << line << " (" << function << ") - " << buffer << std::endl;
	}
} // namespace utils
