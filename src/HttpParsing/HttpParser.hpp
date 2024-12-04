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
# include <iostream>
# include <sstream>
# include <fstream>
# include <map>
# include <unordered_set>
# include <string>
# include <memory>
# include <filesystem>


class ServerSettings;

struct HttpRequest {
	int	method;
	std::string path;
	std::map<std::string, std::string> headers;
	std::string body;
	std::string host;
	bool connection;
	int errorFlag;
	ServerSettings* settings;
	HttpRequest();
};

class ServerSettings;
class LocationSettings;

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
	static void	bigSend(int out_fd, std::unordered_map<std::string, ServerSettings>&);
	void parseClientRequest(const std::vector<char>& clientData, HttpRequest& request, std::unordered_map<std::string, ServerSettings>&);
	void recieveRequest(int out_fd);
	//bool isValidRequestline(std::string, HttpRequest&);
	//void findKeys(HttpRequest& request);
	//void handleChunkedBody(HttpRequest&, std::istringstream&);
	//int hexToInt(std::string);
	void parseBody(HttpRequest&, std::istringstream&);
	void parseRegularBody(std::istringstream&, HttpRequest&);
	void checkForCgi(std::string);
	//void validateLocation(LocationSettings*, int*);
	//std::string trim(const std::string& str);
};