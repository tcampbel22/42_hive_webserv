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

#include "requestLineValidator.hpp"

bool requestLineValidator::isValidRequestline(std::string rLine, HttpRequest& request)
{
	std::string tmp;
	size_t startPos;
	
	size_t spPos = rLine.find(' ');    //find the first space in the RL and check that it's either GET, POST or DELETE request. Anything else it's false
	if (spPos == std::string::npos) {
		request.errorFlag = 1;
		return false;
	}

	tmp = rLine.substr(0, rLine.find(' '));
	//std::cout << "tmp for type: '" << tmp << "'\n"; //test purposes only
	
	static const std::unordered_set<std::string> validMethods = {"GET", "POST", "DELETE"}; //trying out containers not sure if this is allowed
	if (validMethods.find(tmp) == validMethods.end()) {
		//error response here (error 404 bad request or 500 internal server error)
		request.errorFlag = 1;
		return false;
	}
	request.method = tmp;
	
	startPos = spPos + 1;
	spPos = rLine.find(' ', startPos);
	if (spPos == std::string::npos)   //if true no space found
		return false;
	
	tmp = rLine.substr(startPos, spPos - startPos);
	if (tmp.empty() ||  tmp[0] != '/') { // probably needs more checking for the path, but that is the most important check atleast :D. will come back to this.
		//if Path is incorrect: error handling here(HTTP Status 400 or HTTP Status 404).
		request.errorFlag = 1;
		return false;
	}
	request.path = tmp;
	
	startPos = spPos + 1;
	tmp = rLine.substr(startPos, spPos - startPos);     //Version detection, has to be *HTTP/1.1\r*
	if (tmp != "HTTP/1.1\r") {
		//error shit here if version wrong(HTTP 505 - HTTP Version Not Supported)
		request.errorFlag = 1;
		return false;
	}
	return true;
}

const std::unordered_set<std::string> HttpRequestValidator::_validMethods = {
    "GET", "POST", "DELETE" };

