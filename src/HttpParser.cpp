/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eagbomei <eagbomei@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 14:52:49 by tcampbel          #+#    #+#             */
/*   Updated: 2024/11/08 17:21:58 by eagbomei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/HttpParser.hpp"

HttpParser::HttpParser() : _clientDataBuffer(0) {}

HttpParser::~HttpParser() {}


//Reads the client request and stores it in a vector<char>
void	HttpParser::recieveData(int out_fd)
{
	ssize_t infoSize = 1024;
	ssize_t bytesRead = 0;
	
	_clientDataBuffer.resize(infoSize);
	while(true)
	{
		_clientDataBuffer.resize(_clientDataBuffer.size() + infoSize);
		bytesRead = read(out_fd, &_clientDataBuffer[_clientDataBuffer.size() - infoSize], infoSize);
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
	_clientDataBuffer.resize(_clientDataBuffer.size() - (infoSize + bytesRead));
}
//Empty the vector to the requestMap, needs to be parsed in the response.
void HttpParser::parseClientRequest(const std::vector<char>& clientData)
{
    std::string data(clientData.begin(), clientData.end());
    std::istringstream requestStream(data);
    std::string line;
    //For the request format:
    if (std::getline(requestStream, line)) {
        std::istringstream requestLineStream(line);
        std::string type, path, version;
		
        requestLineStream >> type >> path >> version; //splitting request type by whitespaces
        //Hardcoded atm, looking for better solution for the request type
        _requestMap["Type"] = type;
        _requestMap["Path"] = path;
        _requestMap["Version"] = version;
    }
    //Adding the headers to the map and Spliting them by ":" to get key-value pairs.
    while (std::getline(requestStream, line)) {
        ssize_t colonPos = line.find(':');
        if ((size_t)colonPos != std::string::npos) {
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            _requestMap[key] = value;
        }
	}
}



void	HttpParser::bigSend(int out_fd) 
{
	HttpParser parser;
	parser.recieveData(out_fd);
	parser.parseClientRequest(parser._clientDataBuffer);
	//std::string str(parser._clientDataBuffer.begin(), parser._clientDataBuffer.end()); // Convert to string
    //std::cout << "this stuff is in the map\n" << parser._requestMap. << std::endl << std::endl;
	 for (const auto& pair : parser._requestMap) {
        std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
    }
	
	// std::ifstream ifs("./assets/response.html");
	// if (!ifs.is_open())
	// 	std::cerr << "Can't open file\n";
	// std::string buf((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	// ifs.close();
	// if (buf.find("GET"))
	// 	send(out_fd, buf.c_str(), buf.size(), 0);
}


//util function to trim off the white spaces and delimit the read when making key value pair
std::string HttpParser::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}