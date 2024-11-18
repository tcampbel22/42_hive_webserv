/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eagbomei <eagbomei@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 14:43:31 by tcampbel          #+#    #+#             */
/*   Updated: 2024/11/18 11:10:43 by eagbomei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once 

# include "../include/HttpServer.hpp"
# include <iostream>
# include <sstream>
# include <fstream>
# include <map>
# include <unordered_set>


struct HttpRequest {
	std::string method;
	std::string path;
	std::map<std::string, std::string> headers;
	std::string body;
	std::string connection;
	std::string host;
	bool errorFlag;
};

class HttpParser
{
protected:
	//std::map<std::string, std::string> _requestMap;
	std::vector<char> _clientDataBuffer;
public:
	HttpParser();
	~HttpParser();
	static void	bigSend(int out_fd);
	void parseClientRequest(const std::vector<char>& clientData, HttpRequest& request);
	void recieveRequest(int out_fd);
	bool isValidRequestline(std::string, HttpRequest&);
	void findKeys(HttpRequest& request);



	//std::string trim(const std::string& str);
};