/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clundber < clundber@student.hive.fi>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 10:14:15 by clundber          #+#    #+#             */
/*   Updated: 2024/11/04 15:54:36 by clundber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include <string>
# include <sys/socket.h>
# include <iostream>
// # include <stdlib.h>
# include <unistd.h>
# include <netinet/in.h>
# include <sstream>
# include <arpa/inet.h>

class HttpServer
{
private:
	std::string _ipAdress;
	int			_port;
	int			_socket;
	int			_clientSocket;
	sockaddr_in _socketInfo;
	
public:
	//constructors & destructors
	HttpServer(const std::string _ip, uint _port);
	~HttpServer();
	//operator overloads
	
	//methods
	void startServer();
	void closeServer();
	void startListening();
};
