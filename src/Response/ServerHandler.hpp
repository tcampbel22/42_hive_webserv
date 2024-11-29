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
#include <iostream>
#include "Response.hpp"
#include "../HttpParsing/HttpParser.hpp"
#include <unordered_map>
# include "../Config/LocationSettings.hpp"

struct HttpRequest;

class ServerHandler
{
private:
	Response _response;
	HttpRequest& _input;
	bool	home = false;
	LocationSettings *locSettings;

	std::unordered_map<std::string, std::string> MIMEs;

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
	ServerHandler(int fd, HttpRequest& _newInput);
	void getLocationSettings();
	int 	checkMethod();
	void	setContentType(std::string path);
	void 	parsePath();
	void 	executeInput();
	void 	doPost();
	void 	doDelete();
	void 	doGet();
	void 	doError();
	int		getFile(std::string path);
	void	makeMIME();
};