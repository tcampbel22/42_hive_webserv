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
	bool connection;
	int errorFlag;
	ServerSettings* settings;
	HttpRequest(ServerSettings *);
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
public:
	HttpParser();
	~HttpParser();
	static void	bigSend(fdNode*, int, epoll_event&);
	void parseClientRequest(const std::vector<char>& clientData, HttpRequest& request, ServerSettings *);
	//void recieveRequest(int out_fd);
	//bool isValidRequestline(std::string, HttpRequest&);
	//void findKeys(HttpRequest& request);
	//void handleChunkedBody(HttpRequest&, std::istringstream&);
	//int hexToInt(std::string);
	void parseBody(HttpRequest&, std::istringstream&);
	void parseRegularBody(std::istringstream&, HttpRequest&);
	void checkForCgi(ServerSettings* ,std::string);
	void checkRedirect(HttpRequest& request, ServerSettings *);
	//void validateLocation(LocationSettings*, int*);
	//std::string trim(const std::string& str);
};