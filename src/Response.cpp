/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clundber < clundber@student.hive.fi>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 11:34:22 by clundber          #+#    #+#             */
/*   Updated: 2024/11/13 14:15:52 by clundber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"


void Response::sendResponse(int fd)
{
	
}

Response::Response():
_responseCode(999), _contentType(nullptr), _contentLength(0), _body(nullptr)
{
}





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