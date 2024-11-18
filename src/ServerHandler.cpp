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

#include "ServerHandler.hpp"
#include <fstream>
#include <sstream>


ServerHandler::ServerHandler(int fd): //(InputinformationClass& Eromon)
_response()
{
	try
	{
		//InputClass = Eromon; 
		_error = true;
		executeInput();
		_response.setResponseCode(404);
		if (_response.getResponseCode() == 404)
			getFile("root/etc/response/404.html");
		_response.sendResponse(fd);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
}

void ServerHandler::executeInput()
{
	
	if (_error == true)
		doError();
	else if (_type == "POST")
		doPost();
	else if (_type == "GET")
		doGet();
	else if (_type == "DELETE")
		doDelete();
	else
		throw std::invalid_argument("Invalid argument");

}

void ServerHandler::getFile(std::string path)
{
	
	//std::string			buf;
	std::ostringstream	stream;
	std::fstream 		infile;

	infile.open(path);
	if (!infile.is_open())
	{
		//should set flag to failed for status code
		_response.setResponseCode(404);
		//std::cerr << "File failed to open\n";
		return ;
	}
	stream << infile.rdbuf();
	_response.set_body(stream.str());
	infile.close();
	_response.setContentLength(stream.str().length());
	if (path.find(".html"))// needs to be made more robust / to handle others as well, maybe own function
		_response.setContentType("text/html");
}

void ServerHandler::doError()
{
	_response.setResponseCode(400);
	getFile("root/etc/response/400.html");
	//should get a error file from the directory

}

void ServerHandler::doPost()
{
	
}

void ServerHandler::doGet()
{
	
}

void ServerHandler::doDelete()
{
	
}