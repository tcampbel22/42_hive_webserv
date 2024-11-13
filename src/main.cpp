/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eagbomei <eagbomei@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 09:52:16 by clundber          #+#    #+#             */
/*   Updated: 2024/11/13 14:06:35 by eagbomei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/webserv.hpp"
#include "../include/HttpServer.hpp"

int	main(void)
{
	// if (argc != 2)
	// 	return (1);
	// std::string confFile = argv[1];
	//parsing
	
	/*start server class, calls the socket creation function in constructor, closes the socket in
	the destructor.
	*/
	// HttpServer server("127.0.0.1", 8000);
	// server.startListening();
	int fd = open("./test.txt", O_RDONLY);
	HttpParser parser;
	parser.bigSend(fd);
	

	//exit

	return (0);
}