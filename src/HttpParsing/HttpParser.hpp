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

#pragma once 

#include "../../include/webserv.hpp"
# include "../Server/HttpServer.hpp"
# include "../Response/ServerHandler.hpp"
# include "../Config/LocationSettings.hpp"
# include <iostream>
# include <sstream>
# include <fstream>
# include <map>
# include <unordered_set>
# include <string>
# include <memory>
# include <filesystem>

struct fdNode;
class ServerSettings;
class LocationSettings;


struct HttpRequest {
	int	method;
	std::string path;
	std::map<std::string, std::string> headers;
	std::string body;
	std::string host;
	bool closeConnection = false;
	int errorFlag;
	ServerSettings* settings;
	HttpRequest(ServerSettings *, int, epoll_event&);
	bool isCGI;
	int epollFd;
	epoll_event& events;
};


class HttpParser
{
private:
	//std::map<std::string, std::string> _requestMap;
	std::vector<char> _clientDataBuffer;
	bool _fullyRead;
	int _contentLength;
	bool cgiflag;
	std::string query;
	std::string cgiPath;
public:
	HttpParser();
	~HttpParser();
	static int	bigSend(fdNode*, int, epoll_event&);
	void parseClientRequest(const std::vector<char>& clientData, HttpRequest& request, ServerSettings *, HttpParser&);
	//void recieveRequest(int out_fd);
	//bool isValidRequestline(std::string, HttpRequest&);
	//void findKeys(HttpRequest& request);
	//void handleChunkedBody(HttpRequest&, std::istringstream&);
	//int hexToInt(std::string);
	void parseBody(HttpRequest&, std::istringstream&);
	void parseRegularBody(std::istringstream&, HttpRequest&);
	void checkForCgi(std::string, HttpParser&, ServerSettings*);
	void checkRedirect(HttpRequest& request, ServerSettings *);
	uint getContentLength();
	//void validateLocation(LocationSettings*, int*);
	//std::string trim(const std::string& str);
};