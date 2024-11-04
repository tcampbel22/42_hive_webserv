/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clundber < clundber@student.hive.fi>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 09:52:16 by clundber          #+#    #+#             */
/*   Updated: 2024/11/04 10:27:59 by clundber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/webserv.hpp"
#include "../include/HttpServer.hpp"

int	main(int argc, char **argv)
{
	if (argc != 2)
		return (1);
	std::string confFile = argv[1];
	//parsing
	
	/*start server class, calls the socket creation function in constructor, closes the socket in
	the destructor.
	*/
	HttpServer server();


	//exit

	return (0);
}