/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clundber < clundber@student.hive.fi>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 10:36:09 by clundber          #+#    #+#             */
/*   Updated: 2024/11/13 16:28:46 by clundber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
// #include <vector>

class Response
{
private:
	uint _responseCode;
	std::string _contentType;
	uint _contentLength;
	//possibly Date
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
	std::string getResponseCode();
	std::string makeDate();
	void sendResponse(int fd);
};



// A Status-line
// Zero or more header (General|Response|Entity) fields followed by CRLF
// An empty line (i.e., a line with nothing preceding the CRLF) 
// indicating the end of the header fields
// Optionally a message-body