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

HttpRequest::HttpRequest(ServerSettings *serverPtr) : connection(true), errorFlag(-1), settings(serverPtr) {}


//Reads the client request and stores it in a vector<char>
// void	HttpParser::recieveRequest(int out_fd)
// {
// 	ssize_t bytesRead = 0;
// 	size_t bytes = 1024;
// 	_fullyRead = true; //Added so it would compile
	
// 	while(true)
// 	{
// 		_clientDataBuffer.resize(_clientDataBuffer.size() + bytes);
// 		bytesRead = read(out_fd, &_clientDataBuffer[_clientDataBuffer.size() - bytes], bytes);
// 		if (bytesRead < 0) {
// 			// if (errno == EAGAIN || errno == EWOULDBLOCK) {
//             //     std::cout << "Everything read succesfully to the vector" << std::endl;
//             //     break;
// 			// }
//             // std::cerr << "Error receiving data: " << strerror(errno) << std::endl;
//             break ;
//         }
// 		else if (bytesRead == 0) {
// 			std::cout << "Everything read succesfully to the vector" << std::endl;
// 			break ;
// 		}
// 	}
// 	_clientDataBuffer.resize(_clientDataBuffer.size() - (bytes + bytesRead));
// }
//Empty the vector to the requestMap, needs to be parsed in the response.
void HttpParser::parseClientRequest(const std::vector<char>& clientData, HttpRequest& request, ServerSettings *serverPtr)
{
	try {
		std::string data(clientData.begin(), clientData.end());
    	std::istringstream requestStream(data);
    	std::string line;
		//Parse the requestline and store the relevant stuff (path and method)
		if (!std::getline(requestStream, line) || !requestLineValidator::isValidRequestLine(line, request)) {
			request.errorFlag = 400;//error shit in here if first line is bad: ERROR 400 according to RFC
			std::cout << "Error: Could not read the request line or the request line is invalid." << std::endl; 
		}
		checkRedirect(request, serverPtr);
		// checkForCgi(request.path);
		
		HttpHeaderParser::parseHeaders(requestStream, request);
		HttpHeaderParser::procesHeaderFields(request, this->_contentLength);
		if (!HttpHeaderParser::HostParse(serverPtr, request)) {
		 	request.errorFlag = 400;
		 	request.connection = false;
		}
		if (request.method == GET && this->_contentLength != 0) {
			request.errorFlag = 404;
		}
		if (_contentLength || request.headers.find("Transfer-Encoding") != request.headers.end())
			parseBody(request, requestStream);
		} catch (std::exception& e) {
			std::cerr << e.what() << '\n';
		}
}
void HttpParser::checkRedirect(HttpRequest& request, ServerSettings *serverPtr) {
	LocationSettings *block = serverPtr->getLocationBlock(request.path);
	if (!block)
		return;
	if (block->isRedirect())
		request.path = block->getRedirect();
}

void HttpParser::checkForCgi(std::string line) {
	std::regex regex("/cgi-bin/"); //this needs to be changed to be compared to URL
	if (std::regex_search(line, regex))
	{
		cgiflag = true;
	}
	else
		return;
	size_t colPos = line.find('?');
	if (colPos != std::string::npos)
	{
		query = line.substr(colPos);
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
	std::string line;
	char c;
	_contentLength = std::stoi(request.headers.at("Content-Length"));
	for (int i = 0; i < _contentLength && stream.get(c); i++)
		request.body += c;
}


void	HttpParser::bigSend(fdNode *requestNode) 
{
	// auto it2 = settings.find("127.0.0.1:8081");
	// LocationSettings* locptr = serverPtr->getLocationBlock("/");
	// ConfigUtilities::printLocationBlock(*locptr);

	HttpParser parser;
	HttpRequest request(requestNode->serverPtr);
	parser._fullyRead = true;
	//parser.recieveRequest(requestNode->fd);
	parser.parseClientRequest(requestNode->_clientDataBuffer, request, requestNode->serverPtr);
	// if (parser.cgiflag){
	// 	LocationSettings *cgiBlock = request.settings->getCgiBlock();
	// 	if (cgiBlock)
	// 	{
	// 		CGIparsing myCgi("/bin/cgi/cgi.py");
	// 		myCgi.setCGIenvironment(request, parser.query);
	// 		myCgi.execute(request);
	// 	}
	// 	else
	// 		request.errorFlag = 400;
	// }
	//std::cout << request.body;
	// for (const auto& pair : request.headers) {
    //     std::cout << "Key: " << pair.first << " Value: " << pair.second << std::endl;
    // }

	// std::cout << request.body << std::endl;

	// std::string str(parser._clientDataBuffer.begin(), parser._clientDataBuffer.end()); // Convert to string
   	// std::cout << "this stuff is in the map\n" << str << std::endl << std::endl << std::endl << std::endl << "next stuff in the a map\n";
	ServerHandler response(requestNode->fd, request);
}

// util function to trim off the white spaces and delimit the read when making key value pair
// std::string HttpParser::trim(const std::string& str) {
//     size_t first = str.find_first_not_of(" \t\n\r\f\v");
//     size_t last = str.find_last_not_of(" \t\n\r\f\v");
//     return (first == std::string::npos) ? "" : str.substr(first, (last - first + 1));
// }