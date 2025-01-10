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
			Logger::setErrorAndLog(&request.errorFlag, 400, "request-line: syntax error");
			return false;
		}

		tmp = rLine.substr(0, rLine.find(' '));
		if (_validMethods.find(tmp) == _validMethods.end()) {
			if (!request.errorFlag)
				Logger::setErrorAndLog(&request.errorFlag, 405, "request-line: invalid method"); //error response here (error 404 bad request or 500 internal server error)
			return false;
		}
		if (!tmp.compare("GET"))
			request.method = GET;
		if (!tmp.compare("POST"))
			request.method = POST;
		if (!tmp.compare("DELETE"))
			request.method = DELETE;
		startPos = spPos + 1;
		spPos = rLine.find(' ', startPos);
		if (spPos == std::string::npos)
			return false;
		
		tmp = rLine.substr(startPos, spPos - startPos);
		if (tmp.empty() ||  tmp[0] != '/') { // probably needs more checking for the path, but that is the most important check atleast :D. will come back to this.
			if (!request.errorFlag)
				Logger::setErrorAndLog(&request.errorFlag, 400, "request-line: incorrect path"); //if Path is incorrect: error handling here(HTTP Status 400 or HTTP Status 404).
			return false;
		}
		// if (!checkPath(tmp)) { // checking if any there is additional slashes and that it's ascii ('/')
		// 	if (!request.errorFlag)
		// 		Logger::setErrorAndLog(&request.errorFlag, 400, "request-line: path syntax error");
		// 	request.closeConnection = true;
		// }
		normalizePath(tmp);
		request.path = tmp;
		
		startPos = spPos + 1;
		tmp = rLine.substr(startPos, spPos - startPos);     //Version detection, has to be *HTTP/1.1\r*
		if (tmp != "HTTP/1.1\r") {
			if (!request.errorFlag)
				Logger::setErrorAndLog(&request.errorFlag, 505, "request-line: invalid HTTP version"); //error shit here if version wrong(HTTP 505 - HTTP Version Not Supported)
			return false;
		}
	return true;
}

// void requestLineValidator::trimPath(std::string& path) {
// 	if (!checkPath(path))
// 	{
// 		std::string trimmedPath;
// 		bool		lastWasSlash = false;
// 		for (size_t i = 0; i < path.length(); i++) {
// 			if (path[i] == '/') {
// 				if (!lastWasSlash) {
// 					trimmedPath += '/';
// 					lastWasSlash = true;
// 				}
// 			}
// 			else {
// 				trimmedPath += path[i];
// 				lastWasSlash = false;
// 			}
// 		}
// 		if (trimmedPath.length() > 1 && trimmedPath.back() == '/') { //might need to remove, removes the trailing slash ('/')
//             trimmedPath.pop_back();
//         }
// 		// std::cout << trimmedPath << std::endl;
// 		path = trimmedPath;
// 	}
// }

void	requestLineValidator::normalizePath(std::string &path)
{
	size_t pos;

	while ((pos = path.find("//")) != std::string::npos)
		path.erase(pos, 1);
}	

bool requestLineValidator::checkPath(const std::string path) {
	int	count = 0;
	for (size_t i = 0; i < path.length(); i++) {
		if (path[i] && path[i] == '/' && path[i + 1] == '/')
			count++;
	}
	if (count > 0)
		return false;
	if (!path.compare("/"))
		return true ;
	// std::regex regex("^(\\/[a-zA-Z0-9._~-]+(?:%[0-9A-Fa-f]{2}|[a-zA-Z0-9._~-])*)*$");
	// return std::regex_search(path, regex);
	return true;
	
}

