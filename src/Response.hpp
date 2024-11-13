/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clundber < clundber@student.hive.fi>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 10:36:09 by clundber          #+#    #+#             */
/*   Updated: 2024/11/13 14:00:04 by clundber         ###   ########.fr       */
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
	//possibly Date
	std::string _body;


public:	
	Response();
	~Response(){};
	
	//Methods
	void sendResponse(int fd);
};



// A Status-line
// Zero or more header (General|Response|Entity) fields followed by CRLF
// An empty line (i.e., a line with nothing preceding the CRLF) 
// indicating the end of the header fields
// Optionally a message-body