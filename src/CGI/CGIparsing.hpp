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

#include "../HttpParsing/HttpParser.hpp"
#include <sys/wait.h>
#include <fcntl.h>


#define READ_END 0
#define WRITE_END 1

struct HttpRequest;

class CGIparsing
{
private:
	std::string _pathInfo; //path where to upload the cgi file
public:
	CGIparsing(std::string&);
	void setCGIenvironment(HttpRequest& request, const std::string&);
	void execute(HttpRequest&);
	std::string getMethod(int);
	std::string getIp(std::string&);
	std::string getPort(std::string&);
	//std::string
	~CGIparsing();
};

