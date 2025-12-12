#include "http/request.hpp"

#include "utils/logger.hpp"
#include "utils/utils.hpp"
#include <sstream>
#include <algorithm>

namespace http
{
	Request::Request() : _parseState(PARSE_REQUEST_LINE), _methodStr(""), _uri(""), _query(""), _httpVersion(""), _headers(), _body(), _buffer(""), _contentLength(0), _isChunked(false)
	{
	}

	Request::~Request()
	{
	}

	Request::Method Request::parseMethod(const std::string &methodStr)
	{
		if (methodStr == "GET")
			return METHOD_GET;
		else if (methodStr == "POST")
			return METHOD_POST;
		else if (methodStr == "DELETE")
			return METHOD_DELETE;
		else
			return METHOD_UNKNOWN;
	}

	const std::string Request::getHeader(const std::string &key) const
	{
		const std::string lowerKey = key;
		std::transform(lowerKey.begin(), lowerKey.end(), lowerKey.begin(), ::tolower);
		std::map<std::string, std::string>::const_iterator it = _headers.find(lowerKey);
		if (it != _headers.end())
			return it->second;
		return "";
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
			LOG_ERROR("Malformed request line");
			return PARSE_ERROR;
		}

		const Method reqMethod = parseMethod(method);

		size_t posQuery = uri.find('?');
		if (posQuery != std::string::npos)
		{
			_uri = uri.substr(0, posQuery);
			_query = uri.substr(posQuery + 1);
		}
		else
		{
			_uri = uri;
			_query = "";
		}
		_httpVersion = version;
		_methodStr = method;

		return PARSE_HEADERS;
	}

	Request::ParseState Request::parseHeadersLine(const std::string &line)
	{
		if (line.empty())
		{
			const std::string contentLengthStr = getHeader("Content-Length");
			const std::string transferEncoding = getHeader("Transfer-Encoding");
			if (!contentLengthStr.empty())
			{
				_contentLength = std::atol(contentLengthStr.c_str());
				return (_contentLength > 0) ? PARSE_BODY : PARSE_COMPLETE;
			}
			else if (transferEncoding.find("chunked") != std::string::npos)
			{
				_isChunked = true;
				return PARSE_BODY;
			}
			else
			{
				return PARSE_COMPLETE;
			}
		}
		else
		{
			size_t posColon = line.find(':');
			if (posColon == std::string::npos)
			{
				LOG_ERROR("Malformed header line");
				return PARSE_ERROR;
			}

			std::string key = line.substr(0, posColon);
			std::string value = line.substr(posColon + 1);

			utils::trimChars(key);
			utils::trimChars(value);

			std::transform(key.begin(), key.end(), key.begin(), ::tolower);

			_headers.insert(std::make_pair(key, value));
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

			_parseState = parseRequestLine(line);
		}
		if (_parseState == Request::PARSE_HEADERS)
		{
			LOG_DEBUG("Parsing headers");
			while (_parseState != Request::PARSE_BODY && _parseState != Request::PARSE_COMPLETE)
			{
				size_t posEnd = _buffer.find("\r\n");
				if (posEnd == std::string::npos)
				{
					if (_buffer.length() == 0)
						LOG_DEBUG("Headers nothing to parse yet");
					else if (_buffer.length() > 8192)
					{
						LOG_ERROR("Headers too long");
						_parseState = PARSE_ERROR;
						return _parseState;
					}
					else
						LOG_DEBUG("Headers incomplete header line");
					return _parseState;
				}

				const std::string line = _buffer.substr(0, posEnd);
				_buffer.erase(0, posEnd + 2);

				_parseState = parseHeadersLine(line);
			}
		}
		if (_parseState == Request::PARSE_BODY)
		{
			LOG_DEBUG("Parsing body");
		}

		return _parseState;
	} // namespace http
