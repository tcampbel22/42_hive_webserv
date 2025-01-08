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
#include "../HttpParsing/HttpParser.hpp"
#include "../Config/LocationSettings.hpp"
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>



#define READ_END 0
#define WRITE_END 1

struct HttpRequest;
class LocationSettings;

class CGIparsing
{
private:
	std::string _pathInfo; //path where to upload the cgi file
	std::string _execInfo;
public:
	CGIparsing(std::string);
	void setCGIenvironment(HttpRequest& request, const std::string&);
	void execute(HttpRequest&, std::shared_ptr<LocationSettings>&, int, epoll_event&);
	std::string getMethod(int);
	std::string getIp(std::string&);
	std::string getPort(std::string&);
	std::string getPath();
	~CGIparsing();
};

