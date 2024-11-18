/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eagbomei <eagbomei@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 14:52:49 by tcampbel          #+#    #+#             */
/*   Updated: 2024/11/18 11:27:02 by eagbomei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/HttpParser.hpp"

HttpParser::HttpParser()  {}

HttpParser::~HttpParser() {}


//Reads the client request and stores it in a vector<char>
void	HttpParser::recieveRequest(int out_fd)
{
	ssize_t bytesRead = 0;
	
	while(true)
	{
		_clientDataBuffer.resize(_clientDataBuffer.size() + 1);
		bytesRead = read(out_fd, &_clientDataBuffer[_clientDataBuffer.size() - 1], 1);
		if (bytesRead < 0) {
			// if (errno == EAGAIN || errno == EWOULDBLOCK) {
            //     std::cout << "Everything read succesfully to the vector" << std::endl;
            //     break;
			// }
            std::cerr << "Error receiving data: " << strerror(errno) << std::endl;
            break ;
        }
		else if (bytesRead == 0) {
			std::cout << "Everything read succesfully to the vector" << std::endl;
			break ;
		}
	}
	_clientDataBuffer.resize(_clientDataBuffer.size() - (1 + bytesRead));
}
//Empty the vector to the requestMap, needs to be parsed in the response.
void HttpParser::parseClientRequest(const std::vector<char>& clientData, HttpRequest& request)
{
    std::string data(clientData.begin(), clientData.end());
    std::istringstream requestStream(data);
    std::string line;

    //Parse the requestline and store the relevant stuff (path and method)
    if (!std::getline(requestStream, line) || !isValidRequestline(line, request)) {
		//error shit in here if first line is bad: ERROR 400 according to RFC
		std::cout << "Error: Could not read the request line or the request line is invalid." << std::endl;
	}
	/*
		TODO: parse path and method according to config file instructions.
	
	
	
	*/

    // //Parse headers and add them to a map
    while (std::getline(requestStream, line)) {
		ssize_t colonPos = line.find(':');
        if ((size_t)colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            request.headers[key] = value;
        }
	}
	//findKeys(request);
	/* TODO BODY PARSING HERE
	
	
	*/
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
	std::cout << "tmp for Path: '" << tmp << "'\n"; 
	if (tmp.empty() ||  tmp[0] != '/') { // probably needs more checking for the path, but that is the most important check atleast :D. will come back to this.
		//if Path is incorrect: error handling here(HTTP Status 400 or HTTP Status 404).
		request.errorFlag = 1;
		return false;
	}
	request.path = tmp;
	
	startPos = spPos + 1;
	tmp = rLine.substr(startPos, spPos - startPos);     //Version detection, has to be *HTTP/1.1\r*
	std::cout << "tmp for Version: '" << tmp << "'\n";
	if (tmp != "HTTP/1.1\r") {
		//error shit here if version wrong(HTTP 505 - HTTP Version Not Supported)
		request.errorFlag = 1;
		return false;
	}
	
	return true;
}

// void HttpParser::findKeys(HttpRequest& request)
// {
// 	auto key = request.headers;
// }






void	HttpParser::bigSend(int out_fd) 
{
	HttpParser parser;
	HttpRequest request;
	parser.recieveRequest(out_fd);
	parser.parseClientRequest(parser._clientDataBuffer, request);
	// std::string str(parser._clientDataBuffer.begin(), parser._clientDataBuffer.end()); // Convert to string
    // std::cout << "this stuff is in the map\n" << str << std::endl << std::endl << std::endl << std::endl << "next stuff in the a map\n";
	//  for (const auto& pair : request.headers) {
    //     std::cout << "Key: " << pair.first << " Value: " << pair.second << std::endl;
    // }
	
	// std::ifstream ifs("./assets/response.html");
	// if (!ifs.is_open())
	// 	std::cerr << "Can't open file\n";
	// std::string buf((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	// ifs.close();
	// if (buf.find("GET"))
	// 	send(out_fd, buf.c_str(), buf.size(), 0);
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
// }