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
	// if (!serverBlock.back().compare("}"))
	// 	serverBlock.pop_back();
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

void	ConfigUtilities::checkDuplicates(std::variant<int, bool, std::string, std::vector<int>> val, std::string msg)
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
		{
			for (auto it = pair.second.begin(); it != pair.second.end(); it++)
				std::cout << pair.first << ": " << *it << '\n'; 
		}
	}
	std::cout << '\n';
}


void	ConfigUtilities::printLocationBlock(LocationSettings location)
{
	std::cout << "\nLOCATION SETTINGS BLOCK: " << location.getPath() << "\n\n";
	std::cout << "Path: " << location.getPath() << '\n';
	if (!location.getRoot().empty())
		std::cout << "Root: " << location.getRoot() << '\n';
	else
		std::cout << "Root: no root\n";
	std::cout << "Autoindex: " << std::boolalpha << location.isAutoIndex() << '\n';
	if (location.getRedirect().empty())
		std::cout << "Redirect: no redirect\n";
	else
		std::cout << "Redirect: " << location.getRedirect() << '\n';
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
