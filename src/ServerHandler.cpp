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


ServerHandler::ServerHandler(int fd, HttpRequest& _newInput):
_response(), _input(_newInput)
{
	try
	{
		// _input.errorFlag = 1;
		if (_input.path.length() == 1)
			_input.path = _pagePath + _input.path + "/index.html";
		else
			_input.path = _pagePath + _input.path;
		executeInput();
		_response.sendResponse(fd);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
}

void ServerHandler::executeInput()
{
	
	if (_input.path.find("..") != std::string::npos)
	{
		_response.setResponseCode(403);
		getFile("root/etc/response/403.html");
	}
	else if (_input.errorFlag == true)
		doError();
	else if (_input.method == "POST")
		doPost();
	else if (_input.method == "GET")
		doGet();
	else if (_input.method == "DELETE")
		doDelete();
	else
		throw std::invalid_argument("Invalid argument");

}


int ServerHandler::getFile(std::string path)
{
	std::ostringstream	stream;
	std::fstream 		infile;

	infile.open(path);
	if (!infile.is_open())
	{
		//should set flag to failed for status code
		_response.setResponseCode(404);
		getFile("root/etc/response/404.html");
		//std::cerr << "File failed to open\n";
		return (1);
	}
	stream << infile.rdbuf();
	_response.set_body(stream.str());
	infile.close();
	_response.setContentLength(stream.str().length());
	if (path.find(".html"))// needs to be made more robust / to handle others as well, maybe own function
		_response.setContentType("text/html");
	else if (path.find(".webp"))
		_response.setContentType("text/webp");
	else if (path.find(".jpg"))
		_response.setContentType("text/jpg");
	else
		_response.setContentType("text");
		
	return (0);
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
	if (getFile(_input.path) == 1)
		return ;
	_response.setResponseCode(200);
}

void ServerHandler::doDelete()
{
	
}