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

HttpParser::HttpParser() : _fullyRead(true), _contentLength(0) {}

HttpParser::~HttpParser() {}


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
void HttpParser::parseClientRequest(const std::vector<char>& clientData, HttpRequest& request)
{
    try{
		std::string data(clientData.begin(), clientData.end());
    	std::istringstream requestStream(data);
    	std::string line;
		//Parse the requestline and store the relevant stuff (path and method)
		if (!std::getline(requestStream, line) || !isValidRequestline(line, request)) {
			//error shit in here if first line is bad: ERROR 400 according to RFC
			std::cout << "Error: Could not read the request line or the request line is invalid." << std::endl;
		}
		/*
			TODO: parse path and method according to config file instructions. //requires information from config file
		
		
		
		*/

		// //Parse headers and add them to a map, findkeys to store key that casi needs the most for now.
		HttpHeaderParser::parseHeaders(requestStream, request);
		HttpHeaderParser::procesHeaderFields(request, this->_contentLength);
		if (request.method == "GET" && this->_contentLength != 0) {
			request.errorFlag = 404;
		}
		// else {
		// 	if (request.headers.at("Transfer-Encoding").compare(" chunked")) {
		// 			handleChunkedBody(request, requestStream);
		// 	}
		// 	else {
		// 		while (std::getline(requestStream, line)) {
		// 				request.body.append(line + '\n');
		// 		}
		// 	}
		// }
		// std::cout << request.body << std::endl;
	
	} catch (std::exception& e) {
		std::cerr << e.what() << '\n';
	}

}
//*RL = request line
bool HttpParser::isValidRequestline(std::string rLine, HttpRequest& request)
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

void HttpParser::findKeys(HttpRequest& request)
{
	auto it = request.headers.at("Connection");
	if (it.compare("keep-alive"))
		request.connection = false;
	else
		request.connection = true;
	request.host.append(request.headers.at("Host")); //not sure if needed
	try
	{
		_contentLength = std::stoi(request.headers.at("Content-Length"));
	}
	catch(const std::exception& e)
	{
		_contentLength = 0;
		//std::cerr << e.what() << '\n';
	}
}

int HttpParser::hexToInt(std::string line) {
	int 	val;
	try
	{
		val = std::stoi(line, nullptr, 16);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return val;
}

void HttpParser::handleChunkedBody(HttpRequest& request, std::istringstream& stream) 
{
	std::string line;
	while (getline(stream, line))
	{
		if (!line.empty() && line.back() == '\r')
            line.pop_back();
		int chunkSize = hexToInt(line);
		if (chunkSize == 0)
			break ;
		// std::string chunk(chunkSize, '\0');   //not sure if we can use this, since everytime you read you should go through poll
        // stream.read(&chunk[0], chunkSize);
		// std::cout << chunk << std::endl;
        // request.body += chunk;
		getline(stream, line);
		for (int i = 0; i < chunkSize; i++) {
			request.body += line[i]; 
		}
		//stream.ignore(2);
	}
}



void	HttpParser::bigSend(int out_fd) 
{
	HttpParser parser;
	HttpRequest request;
	parser.recieveRequest(out_fd);
	// std::string str(parser._clientDataBuffer.begin(), parser._clientDataBuffer.end()); // Convert to string
   	// std::cout << "this stuff is in the map\n" << str << std::endl << std::endl << std::endl << std::endl << "next stuff in the a map\n";
	parser.parseClientRequest(parser._clientDataBuffer, request);
	//  for (const auto& pair : request.headers) {
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


// void HttpParser::parseClientRequest(const std::vector<char>& clientData)
// {
//     std::string data(clientData.begin(), clientData.end());
//     std::istringstream requestStream(data);
//     std::string line;
//     //For the request format:
//     if (std::getline(requestStream, line)) {
//         std::istringstream requestLineStream(line);
//         std::string type, path, version;
		
//         requestLineStream >> type >> path >> version; //splitting request type by whitespaces
//         //Hardcoded atm, looking for better solution for the request type
//         _requestMap["Type"] = type;
//         _requestMap["Path"] = path;
//         _requestMap["Version"] = version;
//     }
//     //Adding the headers to the map and Spliting them by ":" to get key-value pairs.
//     while (std::getline(requestStream, line)) {
//         ssize_t colonPos = line.find(':');
//         if ((size_t)colonPos != std::string::npos) {
//             std::string key = line.substr(0, colonPos);
//             std::string value = line.substr(colonPos + 1);
//             _requestMap[key] = value;
//         }
// 	}
