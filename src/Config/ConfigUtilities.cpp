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

#include "ConfigUtilities.hpp"

ConfigUtilities::ConfigUtilities() {}
ConfigUtilities::~ConfigUtilities() {}

void	ConfigUtilities::checkBrackets(std::vector<std::string> serverBlock) 
{
	int			lb_count = 0;
	int			rb_count = 0;
	for (auto it = serverBlock.begin(); it != serverBlock.end(); it++)
	{
		if (it->compare("{"))
			lb_count++;
		if (it->compare("}"))
			rb_count++;
	}
	if (lb_count != rb_count)
		throw std::runtime_error("config: curly brackets mismatched");
}

void	ConfigUtilities::trimServerBlock(std::vector<std::string>& serverBlock, std::vector<std::string>::iterator& it)
{
	if (std::next(it) != serverBlock.end() && !it->compare("server") && !std::next(it)->compare("{"))
		it += 2;
}

void	ConfigUtilities::shiftLocationBlock(std::vector<std::string>& location, std::vector<std::string>::iterator& it)
{
	if ((std::next(it) != location.end() || std::next(it)->compare("}") == 0) && !it->compare("{"))
		it++;
	else
		throw std::runtime_error("location: invalid syntax error");

}
void	ConfigUtilities::checkVectorEnd(std::vector<std::string>& vec, std::vector<std::string>::iterator& it, std::string msg)
{
	if (std::next(it) == vec.end())
		throw std::runtime_error(msg);
	else
		it++;
}

void	ConfigUtilities::checkSemiColon(std::vector<std::string>& vec, std::vector<std::string>::iterator it, std::string msg)
{
	if (std::next(it) == vec.end() || (it + 1)->compare(";"))
		throw std::runtime_error(msg);
}

void	ConfigUtilities::checkDuplicates(std::variant<int, bool, std::string, std::vector<int>, std::pair<int, std::string>> val, std::string msg)
{
	if (auto ptr = std::get_if<int>(&val))
	{
		if (*ptr != -1)
			throw std::runtime_error(msg + " duplicate error");
	}
	else if (auto ptr = std::get_if<bool>(&val))
	{
		if (*ptr == true)
			throw std::runtime_error(msg + " duplicate error");
	}
	else if (auto ptr = std::get_if<std::string>(&val))
	{
		if (!ptr->empty())
			throw std::runtime_error(msg + " duplicate error");
	}
	else if (auto ptr = std::get_if<std::vector<int>>(&val))
	{
		if (!ptr->empty())
			throw std::runtime_error(msg + " duplicate error");
	}
	else if (auto ptr = std::get_if<std::pair<int, std::string>>(&val))
	{
		if (ptr->first > 0 && !ptr->second.empty())
			throw std::runtime_error(msg + " duplicate error");
	}

}


void	ConfigUtilities::printServerBlock(ServerSettings server)
{
	
	std::string key(server.getHost() + ":");
	std::cout << "SERVER BLOCK SETTINGS: " << key << server.getPort() << "\n\n";
	std::cout << "Host: " << server.getHost() << '\n';
	std::cout << "Port: " << server.getPort() << '\n';
	std::cout << "Max Client Body: " << server.getMaxClientBody() << '\n';
	std::cout << "Error Pages:\n";
	if (server.getAllErrorPages().empty())
		std::cout << "No error pages\n";
	else
	{
		for (auto& pair : server.getAllErrorPages())
			std::cout << pair.first << ": " << pair.second << '\n'; 
	}
}


void	ConfigUtilities::printLocationBlock(LocationSettings location)
{
	if (!location.getPath().empty())
	{
		std::cout << "\nLOCATION SETTINGS BLOCK: " << location.getPath() << "\n\n";
		std::cout << "Path: " << location.getPath() << '\n';
	}
	if (!location.getRoot().empty())
		std::cout << "Root: " << location.getRoot() << '\n';
	else
		std::cout << "Root: no root\n";
	std::cout << "Autoindex: " << std::boolalpha << location.isAutoIndex() << '\n';
	if (!location.isRedirect())
		std::cout << "Redirect: no redirect\n";
	else
		std::cout << "Redirect: " << location.getRedirect().first << " " << location.getRedirect().second << '\n';
	if (location.getMethods().empty())
		std::cout << "Methods: no methods\n";
	else
	{
		std::cout << "Methods:";
		for (auto it = location.getMethods().begin(); it != location.getMethods().end(); it++) 
		{
			if (*it == 1)
				std::cout << " GET ";
			if (*it == 2)
				std::cout << " POST ";
			if (*it == 3)
				std::cout << " DELETE ";
		}
		std::cout << '\n';
	}
	if (!location.getCgiPath().empty())
		std::cout << "CGI path: " << location.getCgiPath() << '\n';
	else
		std::cout << "CGI path: no cgi_path\n";
	if (!location.getCgiScript().empty())
		std::cout << "CGI script: " << location.getCgiScript() << '\n';
	else
		std::cout << "CGI script: no cgi_script\n";
	if (!location.getCgiUploadPath().empty())
		std::cout << "CGI upload: " << location.getCgiUploadPath() << '\n';
	else
		std::cout << "CGI upload: no upload\n";
	std::cout << '\n';
}

void	ConfigUtilities::checkMethodDuplicates(std::vector<int>& method)
{
	if (std::count(method.begin(), method.end(), GET) > 1)
		throw std::runtime_error("method: duplicate method");
	if (std::count(method.begin(), method.end(), POST) > 1)
		throw std::runtime_error("method: duplicate method");
	if (std::count(method.begin(), method.end(), DELETE) > 1)
		throw std::runtime_error("method: duplicate method");
}


void ConfigUtilities::checkDefaultBlock(ServerSettings block, bool server)
{
	if (server == false)
	{
		block.setDefaultServer(true);
		server = true;
	}
}

bool	ConfigUtilities::checkErrorCode(int code, bool error)
{
	if (error)
	{
		int	error_codes[7] = { 400, 401, 403, 404, 500, 502, 503 };
		for (int i = 0; i < 7; i++)
		{
			if (code == error_codes[i])
				return true;
		}

	}
	else
	{
		if (code >= 300 and code <= 308)
			return true;
	}
	return false;
}