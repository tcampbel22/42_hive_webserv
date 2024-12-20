/**********************************************************************************/
/** __          __  _                                                            **/
/** \ \        / / | |                                by:                        **/
/**  \ \  /\  / /__| |__  ___  ___ _ ____   __                                   **/
/**   \ \/  \/ / _ \ '_ \/ __|/ _ \ '__\ \ / /        Eromon Agbomeirele         **/
/**    \  /\  /  __/ |_) \__ \  __/ |   \ V /         Casimir Lundberg           **/
/**     \/  \/ \___|_.__/|___/\___|_|    \_/          Tim Campbell               **/
/**                                                                              **/
/**                                                                              **/
/**                                W E B S E R V                                 **/
/**********************************************************************************/

#include "HttpHeaderParser.hpp"

void HttpHeaderParser::parseHeaders(std::istringstream& requestStream, HttpRequest& request)
{
	std::string line;
	while (std::getline(requestStream, line)) {
			ssize_t colonPos = line.find(':');
			if ((size_t)colonPos != std::string::npos) {
				std::string key = line.substr(0, colonPos);
				std::string value = line.substr(colonPos + 2); //need to trim whitespace between or no space
				request.headers[key] = value;
			}
			else 
				break;
		}
}

void HttpHeaderParser::procesHeaderFields(HttpRequest& request, int& contentLength)
{
	if (request.headers.find("Connection") != request.headers.end())
	{
		auto it = request.headers.at("Connection");
		if (it.compare("close") == 0)
			request.closeConnection = true;
		else
			request.closeConnection = false;
	}
	if (request.headers.find("Host") != request.headers.end())
		request.host.append((trim(request.headers.at("Host"))));
	try
	{
		request.headers.count("Content-Length");
		contentLength = std::stoi(request.headers.at("Content-Length"));
		if (contentLength > request.settings->getMaxClientBody()) {
			if (!request.errorFlag)
				request.errorFlag = 413;
		}
		if (contentLength < 0)
			contentLength = 0;
	}
	catch(const std::exception& e)
	{
		contentLength = 0;
	}
}
bool HttpHeaderParser::HostParse(ServerSettings* serverPtr, HttpRequest& request) 
{
	if (request.host.find("localhost") != std::string::npos) {
		std::string host = "localhost";
		request.host.erase(0, host.length());
		request.host.insert(0, "127.0.0.1");
	}
	if (request.host  == serverPtr->getKey()) {
		return true;
	}
	return false;
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    return (first == std::string::npos) ? "" : str.substr(first, (last - first + 1));
}

HttpHeaderParser::~HttpHeaderParser() {}