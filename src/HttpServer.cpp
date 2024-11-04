/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clundber < clundber@student.hive.fi>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 10:18:33 by clundber          #+#    #+#             */
/*   Updated: 2024/11/04 16:03:15 by clundber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/HttpServer.hpp"

void HttpServer::startServer()
{
	//make socket
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket == -1)
		return ; //change later
	std::cout << _socket << std::endl;
	
	//store socket addr info
	_socketInfo.sin_family = AF_INET;
	_socketInfo.sin_port = htons(_port);
	_socketInfo.sin_addr.s_addr = inet_addr(_ipAdress.c_str());

	//bind socket to port
	if (bind(_socket, (sockaddr *)&_socketInfo, sizeof(_socketInfo)) < 0)
		return ; //change later and remember to cloase all fds
	
}

void HttpServer::startListening()
{
	if (listen(_socket, 5) < 0)
	{
		return ; //change later, also close all fds
	}
	std::cout << "Server listening on port " << _port << std::endl;
	
	socklen_t _sockLen = sizeof(_socketInfo);
	_clientSocket = accept(_socket, (sockaddr *)&_socketInfo, &_sockLen);
	if (_clientSocket < 0)
		return ; //change later..
	




	close (_clientSocket);
	

}

void HttpServer::closeServer()
{
	close(_socket);
}

HttpServer::~HttpServer()
{
	closeServer();
};
HttpServer::HttpServer(const std::string _ip, uint _newPort)
{
	_port = _newPort;
	_ipAdress = _ip;
	startServer();
	
};
