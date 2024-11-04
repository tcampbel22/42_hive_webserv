/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clundber < clundber@student.hive.fi>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 09:52:16 by clundber          #+#    #+#             */
/*   Updated: 2024/11/04 16:00:08 by clundber         ###   ########.fr       */
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
	HttpServer server("127.0.0.1", 8000);
	server.startListening();

	//exit

	return (0);
}