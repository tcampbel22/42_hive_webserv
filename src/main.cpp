/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcampbel <tcampbel@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 09:52:16 by clundber          #+#    #+#             */
/*   Updated: 2024/11/13 16:07:34 by tcampbel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/webserv.hpp"
#include "../include/HttpServer.hpp"


void	ft_perror(std::string str)
{
	std::cerr << "webserv: " << str << std::endl;
}

int	main(int ac, char **av)
{
	if (ac != 2)
	{
		ft_perror("expecting only configuration file as argument");
		return 1;
	}
	std::string confFile = av[1];
	//parsing
	
	/*start server class, calls the socket creation function in constructor, closes the socket in
	the destructor.
	*/
	HttpServer server("127.0.0.1", 8000);
	server.startListening();

	//exit

	return (0);
}