#include "utils/exception.hpp"

#include <cstdarg>
#include <cstdio>
#include "utils/logger.hpp"

namespace utils
{
	Exception::Exception(const Exception &src) : _message(src._message), _file(src._file), _function(src._function), _line(src._line)
	{
	}

	Exception::Exception(const char *file, const char *function, const int line, const char *format, ...) : _file(file), _function(function), _line(line)
	{
		va_list args;
		va_start(args, format);
		char buffer[1024];
		vsnprintf(buffer, sizeof(buffer), format, args);
		va_end(args);

		_message = std::string(buffer);
	}

	Exception::~Exception() throw() {}

	void Exception::handleException(const std::exception &exception, const char *file, const char *function, int line)
	{
		const Exception *customException = dynamic_cast<const Exception *>(&exception);
		if (customException)
			customException->log();
		else
			Logger::log(Logger::FATAL, file, function, line, "Unhandled exception: %s", exception.what());
	}

	const char *Exception::what() const throw()
	{
		return _message.c_str();
	}

	void Exception::log() const
	{
		Logger::log(Logger::FATAL, _file.c_str(), _function.c_str(), _line, "%s", _message.c_str());
	}
} // namespace utils
