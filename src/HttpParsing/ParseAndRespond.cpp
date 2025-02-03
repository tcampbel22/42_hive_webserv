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

#include "HttpParser.hpp"
#include "HttpHeaderParser.hpp"
#include "requestLineValidator.hpp"
#include "chunkedBodyParser.hpp"
#include "../CGI/CGIparsing.hpp"
#include "../Response/Response.hpp"



void	HttpParser::validateCGIPath(LocationSettings& block, int* error)
{
	if (block.getCgiScript().length() != cgiPath.length()) // Check if there is more in the URI
	{
		pathInfo = cgiPath; //Preserves original path
		if (cgiPath[cgiPath.find(".py") + 3] == '/') 
			cgiPath.erase(cgiPath.find(".py") + 3); //Isolates the path up until the script
	}
	if (!cgiPath.compare(block.getCgiScript()) || cgiPath.find(".py") == std::string::npos)
	{
		Logger::setErrorAndLog(error, 404, "validate-cgi-path: invalid cgi file " + cgiPath);
		return ;
	}
	if (cgiPath.back() == '/')
		cgiflag = false;
	currentCGI = block.getPath();
	cgiflag = true;
}

void	HttpParser::parseQueryString(std::string& path, int *error)
{
	size_t pos = path.find('?');
	if (pos != std::string::npos)
	{
		if (std::count(path.begin(), path.end(), '?') != 1)
			Logger::setErrorAndLog(error, 400, "cgi: Bad query string: " + path);
		query = path.substr(path.find('?') + 1);
		path.erase(pos);
	}
}

void	HttpParser::formatCGIPath(std::string& request_path, LocationSettings& block, HttpRequest& request)
{
	if (request_path.compare(block.getCgiScript()))
	{
		cgiPath = request_path;
		cgiPath.erase(0, block.getPath().length()); //extract the file
		if (cgiPath.front() != '/' && block.getCgiPath().back() != '/') //Add slash too beginning of path
			cgiPath.insert(0, "/");
		cgiPath.insert(0, block.getCgiPath()); // add path from config to get full path
	}
	else
		cgiPath = block.getCgiScript();
	parseQueryString(cgiPath, &request.errorFlag);
	if (!request.errorFlag)
		validateCGIPath(block, &request.errorFlag);
}

void	HttpParser::parseCGI(HttpRequest& request)
{
	std::string key = request.path; //Request path eg cgi-bin/pytester.py
	int len = key.length();
	LocationSettings *locSettings = nullptr;
	while (1) // Check URI against location block to see if it matches a cgi block
	{
		locSettings = request.settings->getLocationBlock(key);
		if (locSettings != nullptr || len < 2)
			break ;
		len--;
		if (len < 1)
			len = 1;
		key = key.substr(0, len);
	}
	if (!locSettings)
		locSettings = request.settings->getLocationBlock("/");
	if (!locSettings)
		return ;
	if (locSettings->isCgiBlock())
		formatCGIPath(request.path, *locSettings, request);
}

// int	HttpParser::bigParse(std::shared_ptr<fdNode> requestNode, HttpRequest& request, HttpServer& server)
// {
// 	HttpParser parser;
// 	if (!request.errorFlag)
// 			parser.parseClientRequest(requestNode->_clientDataBuffer, request, requestNode->serverPtr);
// 	if (parser.cgiflag && !request.errorFlag)
// 	{
// 		LocationSettings* cgiBlock = request.settings->getLocationBlock(parser.currentCGI);
// 		if (cgiBlock && request.method != 3 && requestNode->cgiStarted == false)
// 		{
// 			CGIparsing myCgi(parser.cgiPath, cgiBlock->getCgiScript());
// 			requestNode->path = request.path;
// 			requestNode->method = request.method;
// 			requestNode->childBody = request.body;
// 			myCgi.setCGIenvironment(request, parser);
// 			myCgi.execute(server, requestNode);
// 			return (0);
// 		}
// 		else 
// 		{
// 			Logger::setErrorAndLog(&request.errorFlag, 400, "big send: cgi path not found");
// 			return (1);
// 		}
// 	}
// 	return (0);
// }


// bool HttpParser::handleCGIRepsonse(std::shared_ptr<fdNode> requestNode, HttpRequest& request)
// {
// 	request.errorFlag = requestNode->CGIError;
// 	if (request.errorFlag == 0)
// 	{
// 		request.body = requestNode->CGIBody;
// 		request.method = requestNode->method;
// 		request.path = requestNode->path;
// 		Response response(200, request.body.size(), request.body, request.closeConnection, false);
// 		response.sendResponse(requestNode->fd);
// 		return (0);
// 	}
// 	else
// 	{
// 		ServerHandler Response(requestNode->fd, request);
// 		return (1);
// 	}
// }

// bool	HttpParser::handleResponse(std::shared_ptr<fdNode> requestNode, HttpRequest& request)
// {
// 	if (!requestNode->cgiStarted)
// 		ServerHandler response(requestNode->fd, request);
// 	else
// 		return (0);
// 	return (request.closeConnection) ? 1 : 0;
// }

// bool	HttpParser::bigSend(fdNode *requestNode, HttpServer& server) 
// {
// 	HttpParser parser;
// 	HttpRequest request(requestNode->serverPtr);
// 	request.errorFlag = requestNode->_error;
// 	if (requestNode->CGIReady == true)
// 		return parser.handleCGIRepsonse(requestNode, request);
// 	else
// 	{
// 		if (!request.errorFlag/*  && !requestNode->_readyToSend */)
// 			if (parser.bigParse(requestNode, request, server))
// 				return 1;
// 	}
// 	return parser.handleResponse(requestNode, request);
// }
