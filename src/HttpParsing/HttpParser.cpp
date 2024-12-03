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

HttpParser::HttpParser() : _fullyRead(true), _contentLength(0) {}

HttpParser::~HttpParser() {}

HttpRequest::HttpRequest() : connection(true), errorFlag(-1) {}


//Reads the client request and stores it in a vector<char>
void	HttpParser::recieveRequest(int out_fd)
{
	ssize_t bytesRead = 0;
	size_t bytes = 1024;
	_fullyRead = true; //Added so it would compile
	
	while(true)
	{
		_clientDataBuffer.resize(_clientDataBuffer.size() + bytes);
		bytesRead = read(out_fd, &_clientDataBuffer[_clientDataBuffer.size() - bytes], bytes);
		if (bytesRead < 0) {
			// if (errno == EAGAIN || errno == EWOULDBLOCK) {
            //     std::cout << "Everything read succesfully to the vector" << std::endl;
            //     break;
			// }
            // std::cerr << "Error receiving data: " << strerror(errno) << std::endl;
            break ;
        }
		else if (bytesRead == 0) {
			std::cout << "Everything read succesfully to the vector" << std::endl;
			break ;
		}
	}
	_clientDataBuffer.resize(_clientDataBuffer.size() - (bytes + bytesRead));
}
//Empty the vector to the requestMap, needs to be parsed in the response.
void HttpParser::parseClientRequest(const std::vector<char>& clientData, HttpRequest& request, std::unordered_map<std::string, ServerSettings>& configSettings)
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
		HttpHeaderParser::parseHeaders(requestStream, request);
		HttpHeaderParser::procesHeaderFields(request, this->_contentLength);
		if (!HttpHeaderParser::HostParse(configSettings, request)) {
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

// void HttpParser::validateLocation(LocationSettings* block, int* error) {
// 	std::string path = block->getPath();
// 	std::cout << block->getPath() << std::endl;
// 	if (std::filesystem::exists(path)) {
// 		if (std::filesystem::is_directory(path)){
// 			return;
// 		}
// 		//TBD: might need to add a error or further parsing, if the path exist but is not a directory.
// 	}
// 	else
// 		*error = 404;
// }

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


void	HttpParser::bigSend(int out_fd, std::unordered_map<std::string, ServerSettings>& configSetting) 
{
	HttpParser parser;
	HttpRequest request;
	parser.recieveRequest(out_fd);
	parser.parseClientRequest(parser._clientDataBuffer, request, configSetting);
	// std::string str(parser._clientDataBuffer.begin(), parser._clientDataBuffer.end()); // Convert to string
   	// std::cout << "this stuff is in the map\n" << str << std::endl << std::endl << std::endl << std::endl << "next stuff in the a map\n";

	// std::cout << request.body << std::endl;


	// for (const auto& pair : request.headers) {
    //     std::cout << "Key: " << pair.first << " Value:" << pair.second << std::endl;
    // }


	ServerHandler response(out_fd, request);
}

// util function to trim off the white spaces and delimit the read when making key value pair
// std::string HttpParser::trim(const std::string& str) {
//     size_t first = str.find_first_not_of(" \t\n\r\f\v");
//     size_t last = str.find_last_not_of(" \t\n\r\f\v");
//     return (first == std::string::npos) ? "" : str.substr(first, (last - first + 1));
// }