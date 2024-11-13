/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eagbomei <eagbomei@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 14:43:31 by tcampbel          #+#    #+#             */
/*   Updated: 2024/11/13 14:43:21 by eagbomei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once 

# include "../include/HttpServer.hpp"
# include <iostream>
# include <sstream>
# include <fstream>
# include <map>


struct HttpRequest {
	std::string method;
	std::string path;
	std::map<std::string, std::string> headers;
	std::string body;
};

class HttpParser
{
protected:
	std::map<std::string, std::string> _requestMap;
	std::vector<char> _clientDataBuffer;
public:
	HttpParser();
	~HttpParser();
	static void	bigSend(int out_fd);
	void parseClientRequest(const std::vector<char>& clientData, HttpRequest& request);
	void recieveRequest(int out_fd);
	bool isValidRequestline(std::string);
	



	//std::string trim(const std::string& str);
};