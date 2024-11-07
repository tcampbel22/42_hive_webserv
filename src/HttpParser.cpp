/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eagbomei <eagbomei@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 14:52:49 by tcampbel          #+#    #+#             */
/*   Updated: 2024/11/07 21:41:11 by eagbomei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/HttpParser.hpp"

HttpParser::HttpParser() : _clientDataBuffer(0) {}

HttpParser::~HttpParser() {}

void	HttpParser::recieveData(int out_fd)
{
	ssize_t infoSize = 1;
	ssize_t bytesRead = 0;
	
	_clientDataBuffer.resize(infoSize);
	while(true)
	{
		if ((ssize_t)_clientDataBuffer.size() < bytesRead)
			_clientDataBuffer.resize(_clientDataBuffer.size() * 2);
		
		bytesRead = recv(out_fd, &_clientDataBuffer[_clientDataBuffer.size() - infoSize], infoSize, 0);
		if (bytesRead < 0) {
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

void	HttpParser::bigSend(int out_fd) 
{
	HttpParser parser;
	parser.recieveData(out_fd);
	std::string str(parser._clientDataBuffer.begin(), parser._clientDataBuffer.end()); // Convert to string
    std::cout << "this stuff is in the vector\n" << str << std::endl << std::endl ;
	
	std::ifstream ifs("./assets/response.html");
	if (!ifs.is_open())
		std::cerr << "Can't open file\n";
	std::string buf((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	ifs.close();
	if (buf.find("GET"))
		send(out_fd, buf.c_str(), buf.size(), 0);
}
