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

HttpRequest::HttpRequest(ServerSettings *serverPtr) : closeConnection(false), errorFlag(0), settings(serverPtr) {}

//Empty the vector to the requestMap, needs to be parsed in the response.
void HttpParser::parseClientRequest(const std::vector<char>& clientData, HttpRequest& request, ServerSettings *serverPtr)
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
				Logger::log("parseClientRequest: could not read request line", ERROR);
			else
				Logger::log("parseClientRequest: request line is invalid", ERROR);
			return;
		}
		checkRedirect(request, serverPtr);
		checkForCgi(serverPtr, request.path);
		HttpHeaderParser::parseHeaders(requestStream, request);
		HttpHeaderParser::procesHeaderFields(request, this->_contentLength);
		if (!HttpHeaderParser::HostParse(serverPtr, request) && !request.errorFlag) {
		 	request.errorFlag = 400;
		 	request.closeConnection = true;
		}
		if (request.method == GET && !request.errorFlag)
		{
			std::getline(requestStream, line);
			if (!requestStream.eof()) {
				request.errorFlag = 400;
				return;
			}
			if (this->_contentLength != 0) {
				request.errorFlag = 400;
				return;
			}
		}
		if ((_contentLength || request.headers.find("Transfer-Encoding") != request.headers.end()) && !request.errorFlag) {
			parseBody(request, requestStream);
		}
		} catch (std::exception& e) {
			Logger::log(e.what(), ERROR);
			std::cerr << e.what() << '\n';
		}
}

void HttpParser::checkRedirect(HttpRequest& request, ServerSettings *serverPtr) {
	LocationSettings *block = serverPtr->getLocationBlock(request.path);
	if (!block)
		return;
	if (block->isRedirect())
		request.path = block->getRedirectPath();
}

void HttpParser::checkForCgi(ServerSettings* server, std::string line) {
	std::shared_ptr <LocationSettings> cgibloc = server->getCgiBlock();
	if (!cgibloc)
		return;
	if (!line.compare(server->getCgiBlock()->getCgiScript()))
	{
		cgiflag = true;
	}
	else
		return;
	// size_t colPos = line.find('?');
	// if (colPos != std::string::npos)
	// {
	// 	query = line.substr(colPos);
	// }
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
			request.errorFlag = 400;
		return ;
	}
	if (_contentLength != (int)request.body.size())
	{
		if (!request.errorFlag)
			request.errorFlag = 400;
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
	// std::string str(requestNode->_clientDataBuffer.begin(), requestNode->_clientDataBuffer.end()); // Convert to string
   	// std::cout << "-------------------------------------------------------------------------------------\n\n" << str;
	//parser.recieveRequest(requestNode->fd);
	parser.parseClientRequest(requestNode->_clientDataBuffer, request, requestNode->serverPtr);
	if (parser.cgiflag){
		std::shared_ptr <LocationSettings> cgiBlock = request.settings->getCgiBlock();
		if (cgiBlock)
		{
			CGIparsing myCgi(cgiBlock->getCgiScript());
			myCgi.setCGIenvironment(request, parser.query);
			myCgi.execute(request, cgiBlock, epollFd, _events);
		}
		else
			request.errorFlag = 400;
	}
	//std::cout << request.body;
	// for (const auto& pair : request.headers) {
    //     std::cout << "Key: " << pair.first << " Value: " << pair.second << std::endl;
    // }
	ServerHandler response(requestNode->fd, request);
	if (request.closeConnection == true)
		return (1);
	else
		return (0);
}

// util function to trim off the white spaces and delimit the read when making key value pair
// std::string HttpParser::trim(const std::string& str) {
//     size_t first = str.find_first_not_of(" \t\n\r\f\v");
//     size_t last = str.find_last_not_of(" \t\n\r\f\v");
//     return (first == std::string::npos) ? "" : str.substr(first, (last - first + 1));
// }