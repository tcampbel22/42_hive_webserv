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
				std::string value = line.substr(colonPos + 2);
				request.headers[key] = value;
			}
			else 
				break;
		}
}

void HttpHeaderParser::procesHeaderFields(HttpRequest& request, int& contentLength)
{
	auto it = request.headers.at("Connection");
	if (it.compare("keep-alive"))
		request.connection = false;
	else
		request.connection = true;
	request.host.append(trim(request.headers.at("Host")));
	if (request.headers.count("Content-Length") == 0) {
    	return;
	}
	try
	{
		contentLength = std::stoi(request.headers.at("Content-Length"));
	}
	catch(const std::exception& e)
	{
		contentLength = 0;
	}
}
bool HttpHeaderParser::HostParse(std::unordered_map<std::string, ServerSettings>& config, HttpRequest& request) 
{
	for (auto &it : config) {
		if (it.first == request.host) {
			request.settings = &it.second;
			return true;
		}
	}
	return false;
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    return (first == std::string::npos) ? "" : str.substr(first, (last - first + 1));
}

HttpHeaderParser::~HttpHeaderParser() {}