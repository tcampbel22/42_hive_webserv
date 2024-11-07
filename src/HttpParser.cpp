/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcampbel <tcampbel@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 14:52:49 by tcampbel          #+#    #+#             */
/*   Updated: 2024/11/07 15:34:26 by tcampbel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../include/HttpParser.hpp"


void	HttpParser::bigSend(int out_fd, std::string path) 
{
	std::string			buf;
	std::ostringstream	stream;
	std::fstream 		infile;

	infile.open(path);
	if (!infile.is_open())
	{
		std::cerr << "File failed to open\n";
		return ;
	}
	stream << infile.rdbuf();
	buf = stream.str();
	std::cout << buf << " Buffer\n";
	infile.close();
	if (buf.find("GET"))
		send(out_fd, buf.c_str(), buf.size(), 0);
}