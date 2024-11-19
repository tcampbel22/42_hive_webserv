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
# include <iostream>
# include <sstream>
# include <fstream>
# include <map>
# include <unordered_set>


struct HttpRequest {
	std::string method;
	std::string path;
	std::map<std::string, std::string> headers;
	std::string body;
	std::string host;
	bool connection = true;
	int errorFlag;
};

class HttpParser
{
protected:
	//std::map<std::string, std::string> _requestMap;
	std::vector<char> _clientDataBuffer;
	int _contentLength = 0;
	bool _fullyRead = true; 
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