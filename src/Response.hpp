/************************************************/
/** __          __  _                          **/
/** \ \        / / | |                         **/
/**  \ \  /\  / /__| |__  ___  ___ _ ____   __ **/
/**   \ \/  \/ / _ \ '_ \/ __|/ _ \ '__\ \ / / **/
/**    \  /\  /  __/ |_) \__ \  __/ |   \ V /  **/
/**     \/  \/ \___|_.__/|___/\___|_|    \_/   **/
/**                                            **/
/**                                            **/
/**             W E B S E R V                  **/
/************************************************/

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
