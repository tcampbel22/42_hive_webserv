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
#include "../Server/HttpServer.hpp"
#include "../Response/ServerHandler.hpp"
#include "../Config/LocationSettings.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <unordered_set>
#include <string>
#include <memory>
#include <filesystem>

struct fdNode;
class ServerSettings;
class LocationSettings;
class HttpServer;


struct HttpRequest {
	int	method;
	std::string path;
	std::map<std::string, std::string> headers;
	std::string body;
	std::string host;
	bool closeConnection = false;
	int errorFlag = 0;
	ServerSettings* settings;
	HttpRequest(ServerSettings *);
	bool isCGI;
	~HttpRequest();
};


class HttpParser
{
private:
	std::vector<char> _clientDataBuffer;
	int _contentLength;
	bool cgiflag;
	std::string query;
	std::string cgiPath;
	std::string pathInfo;
	std::string currentCGI;
public:
	HttpParser();
	~HttpParser();
	// static bool	bigSend(std::shared_ptr<fdNode>, HttpServer&);
	static int	bigSend(std::shared_ptr<fdNode>, HttpServer&);
	void 		parseClientRequest(const std::vector<char>& clientData, HttpRequest& request, ServerSettings *);
	void 		parseBody(HttpRequest&, std::istringstream&);
	void 		parseRegularBody(std::istringstream&, HttpRequest&);
	void 		checkForCgi(HttpRequest&, LocationSettings&);
	uint 		getContentLength();
	int			isBlockCGI(HttpRequest&);
	std::string getQuery();
	std::string getPathInfo();
	std::string getCgiPath();
	int			bigParse(std::shared_ptr<fdNode> node, HttpRequest& request, HttpServer& server);
	void		formatCGIPath(std::string& request_path, LocationSettings& block, HttpRequest& request);
	void		parseQueryString(std::string& path, int *error);
	void		parseCGI(HttpRequest& request);
	void		validateCGIPath(LocationSettings& block,  int* error);
	bool		handleCGIRepsonse(std::shared_ptr<fdNode> requestNode, HttpRequest& request);
	bool		handleResponse(std::shared_ptr<fdNode> requestNode, HttpRequest& request);
};