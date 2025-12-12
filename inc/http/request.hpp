#pragma once

#include <string>
#include <map>
#include <vector>

namespace http
{
	class Request
	{
	public:
		enum ParseState
		{
			PARSE_REQUEST_LINE,
			PARSE_HEADERS,
			PARSE_BODY,
			PARSE_COMPLETE,
			PARSE_ERROR
		};

		enum ReqType
		{
			REQ_UNKNOWN,
			REQ_STATIC,
			REQ_CGI,
			REQ_UPLOAD,
			REQ_REDIRECT
		};

	private:
		ParseState _parseState;
		// request line
		std::string _methodStr;
		std::string _uri;
		std::string _query;
		std::string _httpVersion;
		// headers
		std::map<std::string, std::string> _headers;
		// body
		std::vector<char> _body;
		std::string _buffer;

		ParseState parseRequestLine(const std::string &line);

	public:
		Request();
		~Request();

		ParseState parse(const std::string &data, const size_t &len);
	};
} // namespace http
