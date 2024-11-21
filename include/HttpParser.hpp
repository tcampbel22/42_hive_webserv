/************************************************/
/** __          __  _                          **/
/** \ \        / / | |                         **/
/**  \ \  /\  / /__| |__  ___  ___ _ ____   __ **/
/**   \ \/  \/ / _ \ '_ \/ __|/ _ \ '__\ \ / / **/
/**    \  /\  /  __/ |_) \__ \  __/ |   \ V /  **/
/**     \/  \/ \___|_.__/|___/\___|_|    \_/   **/
/**                                            **/
/**                                            **/
/**             W E B S E R V                  **/
/************************************************/

#pragma once 

# include "../include/HttpServer.hpp"
# include "../src/ServerHandler.hpp"
# include <iostream>
# include <sstream>
# include <fstream>
# include <map>
# include <unordered_set>
# include <string>


struct HttpRequest {
	std::string method;
	std::string path;
	std::map<std::string, std::string> headers;
	std::string body;
	std::string host;
	bool connection;
	int errorFlag;
};

class HttpParser
{
private:
	//std::map<std::string, std::string> _requestMap;
	std::vector<char> _clientDataBuffer;
	int _contentLength;
	bool _fullyRead;
public:
	HttpParser();
	~HttpParser();
	static void	bigSend(int out_fd);
	void parseClientRequest(const std::vector<char>& clientData, HttpRequest& request);
	void recieveRequest(int out_fd);
	bool isValidRequestline(std::string, HttpRequest&);
	void findKeys(HttpRequest& request);
	void handleChunkedBody(HttpRequest&, std::istringstream&);
	int hexToInt(std::string);

	//std::string trim(const std::string& str);
};