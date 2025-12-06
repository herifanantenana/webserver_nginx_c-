#pragma once

#include <exception>
#include <string>
#include <cstdarg>

namespace utils
{
	class Exception : public std::exception
	{
	private:
		std::string _message;
		const std::string _file;
		const std::string _function;
		const int _line;

	public:
		static void handleException(const std::exception &exception, const char *file, const char *function, int line);

		Exception(const Exception &src);
		Exception(const char *file, const char *function, const int line, const char *format, ...);

		virtual ~Exception() throw();
		virtual const char *what() const throw();

		void log() const;
	};
}

#define EXCEPTION(...) throw utils::Exception(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define HANDLE_EXCEPTION(e) utils::Exception::handleException(e, __FILE__, __FUNCTION__, __LINE__)
