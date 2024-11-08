/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eagbomei <eagbomei@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 14:43:31 by tcampbel          #+#    #+#             */
/*   Updated: 2024/11/08 16:54:31 by eagbomei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once 

# include "../include/HttpServer.hpp"
# include <iostream>
# include <sstream>
# include <fstream>
# include <map>


class HttpParser
{
protected:
	std::map<std::string, std::string> _requestMap;
	std::vector<char> _clientDataBuffer;
public:
	HttpParser();
	~HttpParser();
	static void	bigSend(int out_fd);
	void parseClientRequest(const std::vector<char>& clientData);
	void recieveData(int out_fd);




	std::string trim(const std::string& str);
};