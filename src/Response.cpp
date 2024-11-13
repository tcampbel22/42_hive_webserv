/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clundber < clundber@student.hive.fi>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 11:34:22 by clundber          #+#    #+#             */
/*   Updated: 2024/11/13 16:31:47 by clundber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <iomanip>
#include <ctime>
#include <sys/socket.h>

Response::Response():
_responseCode(999), _contentLength(0)
{
}

void Response::setResponseCode(uint _code){_responseCode = _code;}

void Response::setContentType(std::string _content){_contentType = _content;}

void Response::setContentLength(uint _length){_contentLength = _length;}

void Response::set_body(std::string _newBody){_body = _newBody;}

void Response::setCloseConnection(bool _close){_closeConnection = _close;}

std::string Response::getResponseCode()
{
	std::string status = "HTTP/1.1 ";
	
	switch (_responseCode)
	{
	case 404:
		status + "404 Not Found\n";
		break;
	default:
		status + "999 status not found\n";
		break;
	}
	return (status); 	
}

std::string Response::getContentType()
{
	std::string _content = "Content-Type: ";
	_content + _contentType + '\n';
	return (_content);
}

std::string Response::getContentLength()
{
	std::string _length = "Content-TLength: ";
	_length + std::to_string(_contentLength) + '\n';
	return (_length);
}

std::string Response::makeDate()
{
	//get the current time from system
	std::time_t _time;
	std::time(&_time);

	//break the time down to its components, and convert to GMT time
	std::tm *_gmtTime = std::gmtime(&_time);
	
	//Date: <day>, <day-of-month> <month> <year> <hour>:<minute>:<second> GMT
	std::string _date = "Date: " + std::to_string(_gmtTime->tm_wday) + ", " + std::to_string(_gmtTime->tm_mday) \
		+ " " + std::to_string(_gmtTime->tm_mon) + " " +  std::to_string(_gmtTime->tm_year) + " " + std::to_string(_gmtTime->tm_hour) \
		+ ":" +std::to_string(_gmtTime->tm_min) + ":" + std::to_string(_gmtTime->tm_sec) + " GMT\n";
	
	return (_date);
}

void Response::sendResponse(int fd)
{
	std::string _buffer = getResponseCode();
	
	if (!_contentType.empty())
	{
		_buffer + getContentType();
		_buffer + getContentLength();
		
	}
	if (_closeConnection == true)
		_buffer + "Connection: close\n";
	_buffer + makeDate();
	_buffer + "\r\n";

	if (!_body.empty())
		_buffer + _body;
	//endline??
	send(fd, _buffer.c_str(), _buffer.size(), 0);
}
