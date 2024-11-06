/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clundber < clundber@student.hive.fi>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 10:14:15 by clundber          #+#    #+#             */
/*   Updated: 2024/11/06 15:23:24 by clundber         ###   ########.fr       */
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
# include <vector>
# include <sys/epoll.h>
# include <fcntl.h>

# define MAX_EVENTS 20 //Can define this in config file or create a funct based on cpu load or leave it

class HttpServer
{
private:
	std::string 	_ipAddress;
	int				_port;
	int				_socket;
	int 			_clientSocket;
	sockaddr_in 	_socketInfo; //reusable
	int				epollFd;
	epoll_event		_events;
	epoll_event		_eventsArr[MAX_EVENTS];
	int				numEvents;
	
	
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
