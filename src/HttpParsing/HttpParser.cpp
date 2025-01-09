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

HttpParser::HttpParser() : _fullyRead(true), _contentLength(0), cgiflag(false), query(""), pathInfo("") {}

HttpParser::~HttpParser() {}

HttpRequest::HttpRequest(ServerSettings *serverPtr, int fd, epoll_event& _event) : closeConnection(false), errorFlag(0), settings(serverPtr), isCGI(false), epollFd(fd), events(_event) {}

//Empty the vector to the requestMap, needs to be parsed in the response.
void HttpParser::parseClientRequest(const std::vector<char>& clientData, HttpRequest& request, ServerSettings *serverPtr, HttpParser& parser)
{
	try {
		std::string data(clientData.begin(), clientData.end());
    	std::istringstream requestStream(data);
    	std::string line;
		//Parse the requestline and store the relevant stuff (path and method)
		if (!std::getline(requestStream, line) || !requestLineValidator::isValidRequestLine(line, request)) {
			//error shit in here if first line is bad: ERROR 400 according to RFC
			if (requestStream.bad() || requestStream.fail())
				Logger::log("parseClientRequest: could not read request line", ERROR, false);
			else
				Logger::log("parseClientRequest: request line is invalid", ERROR, false);
			return;
		}
		checkRedirect(request, serverPtr);
		isBlockCGI(request, parser);
		HttpHeaderParser::parseHeaders(requestStream, request);
		HttpHeaderParser::procesHeaderFields(request, this->_contentLength);
		if (!HttpHeaderParser::HostParse(serverPtr, request) && !request.errorFlag) {
		 	Logger::setErrorAndLog(&request.errorFlag, 400, "header: host name error");
		 	request.closeConnection = true;
		}
		if (request.method == GET && !request.errorFlag)
		{
			std::getline(requestStream, line);
			if (!requestStream.eof()) {
				Logger::setErrorAndLog(&request.errorFlag, 400, "header: body in GET request");
				return;
			}
			if (this->_contentLength != 0) {
				Logger::setErrorAndLog(&request.errorFlag, 400, "header: content-length not zero in GET request");
				return;
			}
		}
		if ((_contentLength || request.headers.find("Transfer-Encoding") != request.headers.end()) && !request.errorFlag) {
			parseBody(request, requestStream);
		}
		} catch (std::exception& e) {
			Logger::log(e.what(), ERROR, false);
		}
}

void HttpParser::checkRedirect(HttpRequest& request, ServerSettings *serverPtr) {
	LocationSettings *block = serverPtr->getLocationBlock(request.path);
	if (!block)
		return;
	if (block->isRedirect())
		request.path = block->getRedirectPath();
}

//tries to get the location settings by using location block matching rules, defaults to / if unsuccessful
int HttpParser::isBlockCGI(HttpRequest& request, HttpParser& parser)
{
	std::string key = request.path;
	int len = 2;
	LocationSettings *locSettings;
	while (42)
	{
		locSettings = request.settings->getLocationBlock(key);
		if (locSettings != nullptr || len < 2)
			break ;
		len = key.rfind('/');
		if (len < 1)
			len = 1;
		key = key.substr(0, len);
	}
	if (!locSettings)
		locSettings = request.settings->getLocationBlock("/");
	if (!locSettings)
		return 0;
	if (locSettings->isCgiBlock() == true)
		checkForCgi(request, parser, *locSettings);
	return 0;
}

//tries to get the location settings by using location block matching rules, defaults to / if unsuccessful
// int HttpParser::isBlockCGI(HttpRequest& request)
// {
// 	std::string key = request.path;
// 	int len = 2;
// 	LocationSettings *locSettings;
// 	while (42)
// 	{
// 		locSettings = request.settings->getLocationBlock(key);
// 		if (locSettings != nullptr || len < 2)
// 			break ;
// 		len = key.rfind('/');
// 		if (len < 1)
// 			len = 1;
// 		key = key.substr(0, len);
// 	}
// 	if (!locSettings)
// 		locSettings = request.settings->getLocationBlock("/");
// 	if (!locSettings)
// 		return 0;
// 	if (locSettings->isCgiBlock() == true)
// 		return 1;
// 	return 0;
// }

void HttpParser::checkForCgi(HttpRequest& request, HttpParser& parser, LocationSettings& cgibloc) {
	// std::shared_ptr <LocationSettings> cgibloc = server->getCgiBlock();
	// if (!cgibloc)
	// 	return;
	std::string location = cgibloc.getPath();
	parser.cgiPath.append(request.path);
	parser.cgiPath.erase(0, cgibloc.getPath().length());
	parser.cgiPath.insert(0, cgibloc.getCgiPath());
	size_t pos = parser.cgiPath.find('?');
	if (pos != std::string::npos)
	{
		if (std::count(parser.cgiPath.begin(), parser.cgiPath.end(), '?') != 1)
			Logger::setErrorAndLog(&request.errorFlag, 400, "cgi: Bad query string");
		parser.query = parser.cgiPath.substr(parser.cgiPath.find('?') + 1);
		parser.cgiPath.erase(pos);
	}
	if (cgibloc.getCgiScript().length() != parser.cgiPath.length())
	{
		parser.pathInfo = parser.cgiPath.substr();
		if (parser.cgiPath[parser.cgiPath.find(".py") + 3] == '/')
			parser.cgiPath.erase(parser.cgiPath.find(".py") + 3);
	}
	std::filesystem::path scriptPath = "." + parser.cgiPath;
	if (std::filesystem::exists(scriptPath)) {
		cgiflag = true;
		std::cout << "path exists" << std::endl;
	}
	else {
		cgiflag = false;
		std::cout << "path doesn't exists" << std::endl;
	}
}

void HttpParser::parseBody(HttpRequest& request, std::istringstream& stream) {
        if (request.headers.count("Transfer-Encoding") == std::string::npos && _contentLength == 0)
			return ;
		auto it = request.headers.find("Transfer-Encoding");
        if (it != request.headers.end() && it->second.find("chunked") != std::string::npos) {
            chunkedBodyParser::parseChunkedBody(stream, request);
        } else {
            parseRegularBody(stream, request);
        }
}

void HttpParser::parseRegularBody(std::istringstream& stream, HttpRequest& request) {
	char c;
	for (int i = 0; i < _contentLength + 1 && stream.get(c); i++) {
		request.body += c;
	}
	if (!stream.eof()) {
		if (!request.errorFlag)
			Logger::setErrorAndLog(&request.errorFlag, 400, "body: no eof in body");
		return ;
	}
	if (_contentLength != (int)request.body.size())
	{
		if (!request.errorFlag)
			Logger::setErrorAndLog(&request.errorFlag, 400, "body: content-length mismatched with body size");
	}
}

int	HttpParser::bigSend(fdNode *requestNode, int epollFd, epoll_event &_events) 
{
	HttpParser parser;
	HttpRequest request(requestNode->serverPtr, epollFd, _events);
	parser._fullyRead = true;
	parser.parseClientRequest(requestNode->_clientDataBuffer, request, requestNode->serverPtr, parser);
	if (parser.cgiflag){
		std::shared_ptr <LocationSettings> cgiBlock = request.settings->getCgiBlock();
		if (cgiBlock && request.method != 3)
		{
			std::cout << cgiBlock->getCgiPath() << std::endl;
			CGIparsing myCgi(parser.cgiPath, cgiBlock->getCgiScript());
			myCgi.setCGIenvironment(request, parser);
			myCgi.execute(request, cgiBlock, epollFd, _events);
			request.isCGI = true;
		}
		else {
			Logger::setErrorAndLog(&request.errorFlag, 400, "big send: cgi path not found");
			return (1);
		}
		if (request.errorFlag == 0) {
			Response response(200, request.body.size(), request.body, request.closeConnection, false);
			response.sendResponse(requestNode->fd);
			return (0);
		}
		else
			request.closeConnection = true;
	}
	ServerHandler response(requestNode->fd, request);
	if (request.closeConnection == true)
		return (1);
	else
		return (0);
}

std::string HttpParser::getQuery() {return query; }
std::string HttpParser::getPathInfo() { return pathInfo; }
uint HttpParser::getContentLength() { return _contentLength; }