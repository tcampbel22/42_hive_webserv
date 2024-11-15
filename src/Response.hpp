/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: casimirri <clundber@student.hive.fi>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 10:36:09 by clundber          #+#    #+#             */
/*   Updated: 2024/11/15 18:19:46 by casimirri        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>

class Response
{
private:
	uint _responseCode;
	std::string _contentType;
	uint _contentLength;
	std::string _body; //could also be vector
	bool _closeConnection = false;

public:	
	Response();
	~Response(){};

	//Setters
	void setResponseCode(uint _code);
	void setContentType(std::string _content);
	void setContentLength(uint _length);
	void set_body(std::string _body);
	void setCloseConnection(bool _close);

	//Methods
	std::string getContentLength();
	std::string getContentType();
	uint getResponseCode();
	std::string getResponseCodeStr();	
	std::string makeDate();
	void sendResponse(int fd);
	std::string makeDay(int day);
	std::string makeMonth(int _month);
};
