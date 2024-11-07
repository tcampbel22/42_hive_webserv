/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcampbel <tcampbel@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 14:52:49 by tcampbel          #+#    #+#             */
/*   Updated: 2024/11/07 15:22:21 by tcampbel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../include/HttpParser.hpp"


void	HttpParser::bigSend(int out_fd, std::string path) 
{
	std::string			buf;
	std::ostringstream	stream;
	std::ifstream ifs;

	ifs.open(path, std::ifstream::in);
	// if (ifs.is_open() == 0)
	// 	return ;
	stream << ifs.rdbuf();
	buf = stream.str();
	std::cout << buf << " Buffer\n";
	ifs.close();
	// if (buf.find("GET"))
		send(out_fd, buf.c_str(), buf.size(), 0);
}