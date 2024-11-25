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

const std::unordered_set<std::string> requestLineValidator::_validMethods = {"GET", "POST", "DELETE"};

bool requestLineValidator::isValidRequestLine(std::string rLine, HttpRequest& request)
{
	std::string tmp;
	size_t startPos;
	
		size_t spPos = rLine.find(' ');    //find the first space in the RL and check that it's either GET, POST or DELETE request. Anything else it's false
		if (spPos == std::string::npos) {
			request.errorFlag = 1;
			return false;
		}

		tmp = rLine.substr(0, rLine.find(' '));
		if (_validMethods.find(tmp) == _validMethods.end()) {
			request.errorFlag = 1; //error response here (error 404 bad request or 500 internal server error)
			return false;
		}
		request.method = tmp;
		
		startPos = spPos + 1;
		spPos = rLine.find(' ', startPos);
		if (spPos == std::string::npos)
			return false;
		
		tmp = rLine.substr(startPos, spPos - startPos);
		if (tmp.empty() ||  tmp[0] != '/') { // probably needs more checking for the path, but that is the most important check atleast :D. will come back to this.
			request.errorFlag = 1; //if Path is incorrect: error handling here(HTTP Status 400 or HTTP Status 404).
			return false;
		}
		request.path = tmp;
		
		startPos = spPos + 1;
		tmp = rLine.substr(startPos, spPos - startPos);     //Version detection, has to be *HTTP/1.1\r*
		if (tmp != "HTTP/1.1\r") {
			request.errorFlag = 1; //error shit here if version wrong(HTTP 505 - HTTP Version Not Supported)
			return false;
		}
	return true;
}



