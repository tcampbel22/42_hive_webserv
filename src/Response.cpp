/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clundber < clundber@student.hive.fi>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 11:34:22 by clundber          #+#    #+#             */
/*   Updated: 2024/11/14 14:20:12 by clundber         ###   ########.fr       */
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
	case 200:
		return(status += "200 OK\n");
	case 301:
		return(status += "301 Moved Permanently\n");
	case 302:
		return(status += "302 Found\n");
	case 400:
		return(status += "400 Bad Request\n");
	case 401:
		return(status += "401 Unauthorized\n");
	case 403:
		return(status += "403 Forbidden\n");		
	case 404:
		return(status += "404 Not Found\n");		
	case 500:
		return(status += "500 Internal Server Error\n");
	case 502:
		return(status += "502 Bad Gateway\n");		
	case 503:
		return(status += "503 Service Unavailable\n");
	default:
		return(status += std::to_string(_responseCode) + " non implemented code\n");
	}
}

std::string Response::getContentType()
{
	std::string _content = "Content-Type: ";
	_content += _contentType + '\n';
	return (_content);
}

std::string Response::getContentLength()
{
	std::string _length = "Content-TLength: ";
	_length += std::to_string(_contentLength) + '\n';
	return (_length);
}

std::string Response::makeDay(int _day)
{	
	switch (_day)
	{
	case 1:
		return ("Mon");
	case 2:
		return ("Tue");
	case 3:
		return ("Wed");
	case 4:
		return ("Thu");
	case 5:
		return ("Fri");
	case 6:
		return ("Sat");
	case 0:
		return ("Sun");		
	default:
		return ("inv");
		break;
	}
}

std::string Response::makeMonth(int _month)
{	
	switch (_month)
	{
	case 0:
		return ("Jan");
	case 1:
		return ("Feb");
	case 2:
		return ("Mar");
	case 3:
		return ("Apr");
	case 4:
		return ("May");
	case 5:
		return ("Jun");
	case 6:
		return ("Jul");	
	case 7:
		return ("Aug");
	case 8:
		return ("Sep");
	case 9:
		return ("Oct");
	case 10:
		return ("Nov");
	case 11:
		return ("Dec");			
	default:
		return ("inv");
		break;
	}
}

std::string Response::makeDate()
{
	//get the current time from system
	std::time_t _time;
	std::time(&_time);

	//break the time down to its components, and convert to GMT time
	std::tm *_gmtTime = std::gmtime(&_time);
	
	//Date: <day>, <day-of-month> <month> <year> <hour>:<minute>:<second> GMT
	std::string _date = "Date: " + makeDay(_gmtTime->tm_wday) + ", " + std::to_string(_gmtTime->tm_mday) \
		+ " " + makeMonth(_gmtTime->tm_mon) + " " +  std::to_string(_gmtTime->tm_year + 1900) + " " + std::to_string(_gmtTime->tm_hour) \
		+ ":" +std::to_string(_gmtTime->tm_min) + ":" + std::to_string(_gmtTime->tm_sec) + " GMT\n";
	
	return (_date);
}

void Response::sendResponse(int fd)
{
	std::string _buffer = getResponseCode();
	
	if (!_contentType.empty())
	{
		_buffer += getContentType();
		_buffer += getContentLength();
	}
	if (_closeConnection == true)
		_buffer += "Connection: close\n";
	_buffer += makeDate();
	_buffer += "\r\n";

	if (!_body.empty())
		_buffer += _body;
	
	//uncomment this in order to see the response in the terminal
	std::cout <<  _buffer << std::endl;
	send(fd, _buffer.c_str(), _buffer.size(), 0);
}
