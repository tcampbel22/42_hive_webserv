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

HttpParser::HttpParser() : _fullyRead(true), _contentLength(0), cgiflag(false), query("") {}

HttpParser::~HttpParser() {}

HttpRequest::HttpRequest(ServerSettings *serverPtr) : closeConnection(false), errorFlag(0), settings(serverPtr), isCGI(false) {}

//Empty the vector to the requestMap, needs to be parsed in the response.
void HttpParser::parseClientRequest(const std::vector<char>& clientData, HttpRequest& request, ServerSettings *serverPtr, HttpParser& parser)
{
	try {
		std::string data(clientData.begin(), clientData.end());
		// ConfigUtilities::printWhiteSpace(data);
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
		checkForCgi(serverPtr, request.path, parser);
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

void HttpParser::checkForCgi(ServerSettings* server, std::string line, HttpParser& parser) {
	std::shared_ptr <LocationSettings> cgibloc = server->getCgiBlock();
	if (!cgibloc)
		return;
	if (line.find('?') != std::string::npos)
	{
		parser.query = line.substr(line.find('?') + 1);
		line.erase(line.find('?'));
	}
	if (!line.compare(server->getCgiBlock()->getCgiScript()))
	{
		cgiflag = true;
	}
	else
		return;
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

	HttpParser parser;
	HttpRequest request(requestNode->serverPtr);
	parser._fullyRead = true;
	//parser.recieveRequest(requestNode->fd);
	parser.parseClientRequest(requestNode->_clientDataBuffer, request, requestNode->serverPtr, parser);
	if (parser.cgiflag){
		std::shared_ptr <LocationSettings> cgiBlock = request.settings->getCgiBlock();
		if (cgiBlock && request.method != 3)
		{
			CGIparsing myCgi(cgiBlock->getCgiScript());
			myCgi.setCGIenvironment(request, parser.query);
			myCgi.execute(request, cgiBlock, epollFd, _events);
			request.isCGI = true;
		}
		else
			Logger::setErrorAndLog(&request.errorFlag, 400, "big send: cgi path not found");
	}
	// std::string str(requestNode->_clientDataBuffer.begin(), requestNode->_clientDataBuffer.end()); // Convert to string
   	// std::cout << "-------------------------------------------------------------------------------------\n\n" << str;
	// for (const auto& pair : request.headers) {
    //     std::cout << "Key: " << pair.first << " Value: " << pair.second << std::endl;
    // }
	std::cout << request.body << std::endl;
	ServerHandler response(requestNode->fd, request);
	if (request.closeConnection == true)
		return (1);
	else
		return (0);
}
