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
#include <iostream>
#include "Response.hpp"
#include "../include/HttpParser.hpp"

struct HttpRequest;

class ServerHandler
{
private:
	HttpRequest& _input;
	Response _response;
	std::string _responsePath;
	std::string _pagePath = "root/var/html";
// struct HttpRequest {
// 	std::string method;
// 	std::string path;
// 	std::map<std::string, std::string> headers;
// 	std::string body;
// 	std::string connection;
// 	std::string host;
// 	int errorFlag;
// };


public:
	ServerHandler(int fd, HttpRequest& _newInput);//(InputinformationClass& Eromon)
	void executeInput();
	void doPost();
	void doDelete();
	void doGet();
	void doError();
	int getFile(std::string path);
};