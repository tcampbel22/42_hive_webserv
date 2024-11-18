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
# include <iostream>
#include "Response.hpp"

// class Response;

class ServerHandler
{
private:
	//InputinformationClass& Eromon;	
	// std::string _accept[20];
	std::string _type = "POST";
	bool	_error = false;
	Response _response;
	// std::string _body;
	// bool	_connection:
	


public:
	ServerHandler(int fd);//(InputinformationClass& Eromon)
	void executeInput();
	void doPost();
	void doDelete();
	void doGet();
	void doError();
	void getFile(std::string path);
};