/**********************************************************************************/
/** __          __  _                                                            **/
/** \ \        / / | |                                by:                        **/
/**  \ \  /\  / /__| |__  ___  ___ _ ____   __                                   **/
/**   \ \/  \/ / _ \ '_ \/ __|/ _ \ '__\ \ / /        Eromon Agbomeirele         **/
/**    \  /\  /  __/ |_) \__ \  __/ |   \ V /         Casimir Lundberg           **/
/**     \/  \/ \___|_.__/|___/\___|_|    \_/          Tim Campbell               **/
/**                                                                              **/
/**                                                                              **/
/**                                W E B S E R V                                 **/
/**********************************************************************************/

#include "ServerHandler.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <regex>
#include <string>

ServerHandler::ServerHandler(int fd, HttpRequest& _newInput):
_response(), _input(_newInput)
{

	// std::cout << _input.path << std::endl;
	// std::cout << "ERROR CODE FROM PARSING = " << _input.errorFlag << std::endl;
	// std::cout << "error flag = " << _input.errorFlag << std::endl;
	try
	{
		makeMIME();
		if (_input.errorFlag < 0)
		{
			parsePath();
		}
		executeInput();
		_response.sendResponse(fd);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
}

//check to see if the path domain specified is located in the server
//and parse the root to it

int ServerHandler::checkMethod()
{
	std::vector<int> allowedMethods = locSettings->getMethods();
	for (const auto& method : allowedMethods)
	{
		if (_input.method == method)
			return 0;
	}
	return (1);
}

void ServerHandler::parsePath()
{
	if (_input.path.empty())
	{
		_input.errorFlag = 400;
		return ;
	}
	int len = 1; 
	if (_input.path.find("/", 1) != std::string::npos)
		len = _input.path.find("/", 1);
	std::string key = _input.path.substr(0, len);
	// std::cout << "locationKey = " << key << std::endl;
	locSettings = _input.settings->getLocationBlock(key);
	if (!locSettings)
	{
		_input.errorFlag = 404;
		return;
	}
	else
		_input.path = locSettings->getRoot() + _input.path;
	if (_input.path.length() > 1 && _input.path.at(0) == '/')
		_input.path = _input.path.substr(1, _input.path.length() -1);
	// std::cout << "DEFAULT FILE = " << locSettings->getDefaultFilePath() << std::endl;
	if (_input.path.back() == '/')
		_input.path = _input.path + locSettings->getDefaultFile();
	// std::cout << _input.path << std::endl;

	std::regex validPathRegex("^[a-zA-Z0-9/_.-]+$");
	if (!std::regex_match(_input.path, validPathRegex))
		_input.errorFlag = 401;		
	else if (_input.path.find("..") != std::string::npos)
		_input.errorFlag = 401;
	if (checkMethod())
		_input.errorFlag = 403;
}


void ServerHandler::executeInput()
{
	
	if (_input.errorFlag == true)
		doError();
	else if (_input.method == POST)
		doPost();
	else if (_input.method == GET)
		doGet();
	else if (_input.method == DELETE)
		doDelete();
	else
		throw std::invalid_argument("Invalid argument");

}
void	ServerHandler::makeMIME()
{
		MIMEs = {
		//Text files
		{".html", "text/html"},
		{".txt", "text/plain"},
		{".css", "text/css"},
		{".js", "text/javascript"}, //could also be application/javascript

		//Image files
		{".webp", "image/webp"},
		{".jpg", "image/jpeg"},
		{".jpeg", "image/jpeg"},
		{".png", "image/png"},
		{".gif", "image/gif"},
		{".svg", "image/svg+xml"},
		{".ico", "image/x-icon"},

		//Video files
		{".mp4", "video/mp4"},
		{".webm", "video/webm"},

		//Audio files
		{".mp3", "audio/mpeg"},
		{".waw", "audio/waw"},

		//Application files
		{".json", "application/json"},
		{".xml", "application/xml"},
		{".pdf", "application/pdf"},
		{".zip", "application/zip"},
		{".gzip", "application/gzip"},
		//Font files
		{".woff", "font/woff"},
		{".woff2", "font/woff2"},
		{".ttf", "font/ttf"},
		{".otf", "font/otf"}
	};
}

void	ServerHandler::setContentType(std::string path)
{
	//generating the key from the extension of the file
	std::string key = path.substr(path.find_last_of("."), path.length() - path.find_last_of("."));
	//makes the key lowercase before searching
	std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) { return std::tolower(c); });

	for (const auto& pair : MIMEs)
	{
		if (key == pair.first)
		{
			_response.setContentType(pair.second);
			return ; 
		}
	}
	//default if no file extension
	_response.setContentType("text/html");
}

int ServerHandler::getFile(std::string path)
{
	std::ostringstream	stream;
	std::fstream 		infile;

	//check for file and for read permission and set error code accordingly
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
	setContentType(path);
		
	return (0);
}

void ServerHandler::doError()
{
	_response.setResponseCode(_input.errorFlag);
	// getFile(locSettings->//get error path)
	getFile("root/etc/response/" +  std::to_string(_input.errorFlag) + ".html");
	//should get a error file from the directory
}

void ServerHandler::doPost()
{
	std::filesystem::path path(_input.path);
	if(std::filesystem::exists(path))
	{
		//file exists, add to it.

		//first check for write permission

	}
	else
	{

		//create file, and stream the body into it
	}
	//in both cases, check if it was successfull, or not, and set response code accordingly
	//mutex might be needed if multiple servers have access to same resources?
	std::cout << "GOT TO POST\n";
	_response.setResponseCode(200);
}


void ServerHandler::doGet()
{
	if (getFile(_input.path) == 1)
		return ;
	_response.setResponseCode(200);
}

void ServerHandler::doDelete()
{
	std::cout << "GOT TO DELETE\n";
	_response.setResponseCode(200);
}