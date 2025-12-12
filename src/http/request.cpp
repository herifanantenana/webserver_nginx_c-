#include "http/request.hpp"

#include "utils/logger.hpp"
#include <sstream>

namespace http
{
	Request::Request() : _parseState(PARSE_REQUEST_LINE), _methodStr(""), _uri(""), _query(""), _httpVersion(""), _headers(), _body(), _buffer("")
	{
	}

	Request::~Request()
	{
	}

	Request::ParseState Request::parseRequestLine(const std::string &line)
	{
		std::istringstream stream(line);
		std::string method;
		std::string uri;
		std::string version;

		stream >> method >> uri >> version;
		if (method.empty() || uri.empty() || version.empty())
		{
			// todo: continue parser
		}
	}

	Request::ParseState Request::parse(const std::string &data, const size_t &len)
	{
		LOG_DEBUG("Request parse called with data length: %zu", len);
		_buffer.append(data, len);

		if (_parseState == Request::PARSE_REQUEST_LINE)
		{
			LOG_DEBUG("Parsing request line");

			size_t posEnd = _buffer.find("\r\n");
			if (posEnd == std::string::npos)
			{
				if (_buffer.length() == 0)
					LOG_DEBUG("Request nothing to parse yet");
				else if (_buffer.length() > 4096)
				{
					LOG_ERROR("Request line too long");
					_parseState = PARSE_ERROR;
					return _parseState;
				}
				else
					LOG_DEBUG("Request incomplete request line");
				return _parseState;
			}

			const std::string line = _buffer.substr(0, posEnd);
			_buffer.erase(0, posEnd + 2);
		}
	}
} // namespace http
