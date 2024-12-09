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
	// ConfigUtilities::printLocationBlock(*locSettings);
	std::vector<int> allowedMethods = locSettings->getMethods();
	for (auto& method : allowedMethods)
	{
		if (_input.method == method)
			return 0;
	}
	// if (_input.method == GET && home == true)
	// 	return 0;
	return (1);
}

//tries to get the location settings by using location block matching rules, defaults to / if unsuccessful
void ServerHandler::getLocationSettings()
{
	int len = _input.path.rfind('/');
	if (len < 1)
		len = 1;
	std::string key = _input.path.substr(0, len);
	while (42)
	{
		std::cout << "here\n";
		locSettings = _input.settings->getLocationBlock(key);
		// std::cout << "here2\n";	// std::cout << "KEY = " << key << std::endl;
		if (locSettings || len < 2)
			break ;		
		
		len = key.rfind('/');
		if (len < 1)
			len = 1;
		key = key.substr(0, len);
	}

}

void ServerHandler::parsePath()
{
	if (_input.path.empty())
	{
		_input.errorFlag = 400;
		return ;
	}
	std::regex validPathRegex("^[a-zA-Z0-9/_.-]+$");
	if (!std::regex_match(_input.path, validPathRegex) || _input.path.find("..") != std::string::npos \
		|| _input.path.find("//") != std::string::npos)
	{
		_input.errorFlag = 401;		
		return ;
	}
	getLocationSettings();
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
	{
		//use bool from config to see if there is a deafault file & if directory listing is on
		_input.path = _input.path + locSettings->getDefaultFile();
	}
	// std::cout << _input.path << std::endl;

	//sanitize the path, and set error if needed
	if (checkMethod())
		_input.errorFlag = 403;
}


void ServerHandler::executeInput()
{
	if (_input.errorFlag > 0)
	{
		doError();
		return ;
	}
	else if (_input.method == POST)
		doPost();
	else if (_input.method == GET)
		doGet();
	else if (_input.method == DELETE)
		doDelete();
	else
		throw std::invalid_argument("Invalid argument");
	if (_input.errorFlag > 0)
		doError();
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
	//default if no file extension // we need to decide the base
	_response.setContentType("text/plain");
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
		// _input.errorFlag = 404;
		// _response.setResponseCode(404);
		return (1);
	}
	stream << infile.rdbuf();
	_response.set_body(stream.str());
	infile.close();
	_response.setContentLength(stream.str().length());
	setContentType(path);
		
	return (0);
}

void ServerHandler::defaultError()
{
	std::string code = std::to_string(_input.errorFlag);
	std::string body;

	body = "	<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"UTF-8\">\n    <title>" + code + "</title>\n</head>\n<body>\n    <h1>";
	body += code + " Error</h1>\n</body>\n</html>";

	_response.set_body(body);
	_response.setContentType("text/html");
	_response.setContentLength(body.length());
}

void ServerHandler::doError()
{
	_response.setResponseCode(_input.errorFlag);

	std::string errorPath;
	// std::vector<std::string> errorVector;
	//could getErrorPages perhaps return just a string path to the correct Error page?
	//check if there are location level error pages for the requested code

	try
	{
		errorPath = locSettings->getErrorPagePath(_input.errorFlag);
	}
	catch(const std::exception& e)
	{
	}
	if (!errorPath.empty())
	{
		//check that the path is valid
		if (getFile(errorPath) == 0)
			return;
	}

	//if no location level pages, check if there is server level pages
	try
	{
		errorPath = _input.settings->getErrorPages(_input.errorFlag);
	}
	catch(const std::exception& e)
	{
	}
	if (!errorPath.empty())
	{
		//check that the path is valid
		if (getFile(errorPath) == 0)
			return;
	}

	//This needs to check the error pyramid for correct error file
	
	// getFile("root/etc/response/" +  std::to_string(_input.errorFlag) + ".html");
	
	//should get a error file from the directory


	//location based error pages not yet set up, but they should be checked first

	//then server based error pages

	//and finally, if no pages found, generate deafult pages;
	defaultError();
}

void ServerHandler::doPost()
{
	std::filesystem::path path(_input.path);
	std::filesystem::path dirPath(_input.path.substr(0, _input.path.rfind('/')));
	//check if the path ends with / meaning its a directory
	if (_input.path.back() == '/')
	{
		_input.errorFlag = 403;
		return ;
	}
	//first check if the directory where the file is / is to be created exists
	if(std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath))
	{	
		//then check if the file exists or not
		if(std::filesystem::exists(path))
		{
			std::ofstream file(_input.path, std::ios::app); //to append to a existing file // will also create the file if it dose not exist
			if (!file.is_open())
			{
				_input.errorFlag = 401;
				return ;
			}
			file << _input.body;
			file.close();
			//first check for write permission ?
		}
		else
		{
			std::ofstream new_file(_input.path);
			if (!new_file.is_open())
			{
				_input.errorFlag = 500;
				return ;
			}
			new_file << _input.body;
			new_file.close();
		}
		//mutex might be needed if multiple servers have access to same resources?
		_response.setResponseCode(200);
	}
	else
		_input.errorFlag = 404;
}

void ServerHandler::generateIndex()
{
	std::filesystem::path dirPath(_input.path);
	std::string body;

	body = "	<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"UTF-8\">\n    <title>" + _input.path + " Directory listing</title>\n</head>\n<body>\n    <h1>";
	body += _input.path + " directory listing:</h1>\n<ul>";

	try
	{
		if(std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath))
		{
			for (const auto& file : std::filesystem::directory_iterator(dirPath))
			{
				if (file.is_directory())
					body += "    <li>[DIR] <a href=\"" + file.path().filename().string() + "/\">" + file.path().filename().string();
				else
					body += "    <li>[FILE] <a href=\"" + file.path().filename().string() + "\">" + file.path().filename().string();
				body += "</a></li>\n";
			}
		}
	}
	catch(const std::exception& e)
	{
		_input.errorFlag = 403; //probably other code
		return ;
	}
	body += "</ul>\n</body>\n</html>";

	_response.set_body(body);
	_response.setContentType("text/html");
	_response.setContentLength(body.length());
	_response.setResponseCode(200);
}

void ServerHandler::doGet()
{
	if (_input.path.back() == '/' && locSettings->isAutoIndex() == true)
	{
		generateIndex();
		return;
	}
	//check if it is asking for a directory and if autoindex is on
	//if so, generate the directory index

	if (getFile(_input.path) == 1)
	{
		if (_input.errorFlag < 0)
			_input.errorFlag = 404;
		return ;
	}
	_response.setResponseCode(200);
}

void ServerHandler::doDelete()
{
	std::filesystem::path path(_input.path);
	std::filesystem::path dirPath(_input.path.substr(0, _input.path.rfind('/')));
	//check if the path ends with / meaning its a directory
	if (_input.path.back() == '/')
	{
		_input.errorFlag = 403;
		return ;
	}
	//first check if the directory where the file is exists	
	if(std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath))
	{	
		//then check if the file exists or not
		if(std::filesystem::exists(path))
		{
			try
			{
				if (!std::filesystem::remove(path))
				{
					_input.errorFlag = 403;
					return ;
				}
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
		}
		else
		{
			_input.errorFlag = 404;
			return ;
		}
		//mutex might be needed if multiple servers have access to same resources?
		_response.setResponseCode(200);
	}
	else
		_input.errorFlag = 404;
	// std::cout << "GOT TO DELETE\n";
}