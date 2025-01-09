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

HttpParser::HttpParser() : _fullyRead(true), _contentLength(0), cgiflag(false), query("") {}

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
		if (isBlockCGI(request))
			checkForCgi(request.path, parser, serverPtr);
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
int HttpParser::isBlockCGI(HttpRequest& request)
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
		return 1;
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

void HttpParser::checkForCgi(std::string line, HttpParser& parser, ServerSettings* server) {
	std::cout << "Line: " + line << '\n';
	std::shared_ptr <LocationSettings> cgibloc = server->getCgiBlock();
	if (!cgibloc)
		return;
	//change location
	std::string location = server->getCgiBlock()->getPath();
	parser.cgiPath.append(line);
	size_t pos = parser.cgiPath.find(location);
	if (pos != std::string::npos)
	{
		parser.cgiPath.erase(1, pos);
		std::cout << "cgi-bin removal: " << server->getCgiBlock()->getPath() << std::endl;
		parser.cgiPath.insert(0, server->getCgiBlock()->getCgiPath());
	}
	std::cout << parser.cgiPath << std::endl;
	//remove extract query: //cgi-bin/cgitester.py/eromon?lol=hello
	if (parser.cgiPath.find('?') != std::string::npos)
	{
		parser.query = line.substr(line.find('?') + 1);
		parser.cgiPath.erase(line.find('?') + 1);
	}
	std::cout << "path after query removal: " << parser.cgiPath << std::endl;
	pos = parser.cgiPath.find_last_of('/');
	if (pos != std::string::npos) {
		parser.pathInfo = parser.cgiPath.substr(pos + 1);
		parser.cgiPath.erase(pos);
	}
	//std::cout << "path cgi path: " << parser.cgiPath << std::endl;
	//std::cout << "path query: " << parser.query << std::endl;
	//std::cout << "path info: " << parser.pathInfo << std::endl;
	// if (!line.compare(server->getCgiBlock()->getCgiScript()))
	// {
	// 	cgiflag = true;
	// }
	// else
	// 	return;
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
	// auto it2 = settings.find("127.0.0.1:8081");
	// LocationSettings* locptr = serverPtr->getLocationBlock("/");
	// ConfigUtilities::printLocationBlock(*locptr);

	// std::string str(requestNode->_clientDataBuffer.begin(), requestNode->_clientDataBuffer.end()); // Convert to string
   //	std::cout << "-------------------------------------------------------------------------------------\n\n" << str;
	HttpParser parser;
	HttpRequest request(requestNode->serverPtr, epollFd, _events);
	parser._fullyRead = true;
	//parser.recieveRequest(requestNode->fd);
	parser.parseClientRequest(requestNode->_clientDataBuffer, request, requestNode->serverPtr, parser);
	if (parser.cgiflag){
		std::shared_ptr <LocationSettings> cgiBlock = request.settings->getCgiBlock();
		if (cgiBlock && request.method != 3)
		{
			std::cout << cgiBlock->getCgiPath() << std::endl;
			CGIparsing myCgi(cgiBlock->getCgiPath(), cgiBlock->getCgiScript());
			myCgi.setCGIenvironment(request, parser.query);
			myCgi.execute(request, cgiBlock, epollFd, _events);
			request.isCGI = true;
		}
		else {
			Logger::setErrorAndLog(&request.errorFlag, 400, "big send: cgi path not found");
			return (1);
		}
		if (request.errorFlag != 0) {
			Response response(200, request.body.size(), request.body, request.closeConnection, false);
			response.sendResponse(requestNode->fd);
			return (0);
		}
		return (1);
	}
	// for (const auto& pair : request.headers) {
    //     std::cout << "Key: " << pair.first << " Value: " << pair.second << std::endl;
    // }
	// std::cout << "error code from parser: " << request.errorFlag << std::endl;
	ServerHandler response(requestNode->fd, request);
	if (request.closeConnection == true)
		return (1);
	else
		return (0);
}

uint HttpParser::getContentLength() { return _contentLength; }