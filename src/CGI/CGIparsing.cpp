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

#include "CGIparsing.hpp"

CGIparsing::CGIparsing(std::string& cgiPath) : _pathInfo(cgiPath) {}

void CGIparsing::setCGIenvironment(HttpRequest& request, const std::string& queryStr) {
	setenv("REQUEST_METHOD", getMethod(request.method).c_str(), 1);
	setenv("QUERY_STRING", queryStr.c_str(), 1);
	setenv("CONTENT_TYPE", "text/html", 1); //default text, needs parsing for images etc.
	setenv("CONTENT_LENGTH", request.headers.at("Content-Length").c_str(), 1);
	//setenv("SERVER_NAME", "localhost", 1); not doing names;
	setenv("SERVER_PORT", getPort(request.host).c_str(), 1);
	setenv("REMOTE_ADDR", getIp(request.host).c_str(), 1);
}

std::string CGIparsing::getMethod(int val) {
	switch (val)
	{
	case 1:
		return "GET";
	case 2:
		return "POST";
	default:
		break;
	}
	return (NULL);
}

std::string CGIparsing::getIp(std::string& host) {
	size_t colPos = host.find(':');
	if (colPos != std::string::npos)
		return host.substr(0, colPos);
	return nullptr;	
}

std::string CGIparsing::getPort(std::string& host) {
	size_t colPos = host.find(':');
	if (colPos != std::string::npos)
		return host.substr(colPos + 1);
	return nullptr;
}

CGIparsing::~CGIparsing() {}