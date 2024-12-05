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

#include "ServerSettings.hpp"
#include "ConfigParser.hpp"

ServerSettings::ServerSettings() : _isDefaultServer(false), host(""), port(-1), max_client_body_size(-1) {}

ServerSettings::ServerSettings(std::string& key) : _isDefaultServer(false), _key(key), host(""), port(-1), max_client_body_size(-1) {}

ServerSettings::~ServerSettings() {}


void	ServerSettings::parseServerBlock(std::vector<std::string>& serverBlock, std::vector<std::string>::iterator& it, std::vector<std::string>::iterator end)
{
	ConfigUtilities::trimServerBlock(serverBlock, it);
	const std::string	directives[6] = {"host", "port", "client_max_body_size", "server_names", "location", "error_page"};
	bool				isValid = false;
	bool				dupServerName = false;
	
	for (; it != end; it++)
	{
		isValid = false;
		for (int i = 0; i < 6; i++)
		{
			if (!it->compare(directives[i]))
			{
				isValid = true;
				switch (i + 1)
				{
				case 1: //host
					parseHost(serverBlock, it);
					break;
				case 2: //port
					parsePort(serverBlock, it);
					break;
				case 3: //body size
					parseMaxBodySize(serverBlock, it);
					break;
				case 4: //server_names
					parseServerNames(serverBlock, it, &dupServerName);
					break;
				case 5: //location block
					parseLocationBlock(serverBlock, it);
					break;
				case 6: //error page
					parseErrorPages(serverBlock, it);
					break;
				default:
					throw std::logic_error("Something went very wrong....");
				break ;
				}
			}
		}
		if (!isValid)
			throw std::runtime_error("invalid directive in server block");
	}
	checkConfigValues(serverBlock, it);
}

void	ServerSettings::parseHost(std::vector<std::string>& directive, std::vector<std::string>::iterator& it) 
{
	ConfigUtilities::checkDuplicates(host, "host:");
	ConfigUtilities::checkVectorEnd(directive, it, "host: empty value");
	ConfigUtilities::checkSemiColon(directive, it, "host: syntax error");
	std::string ip = *it;
	std::regex ip_addr("^((25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]?[0-9])\\.){3}(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]?[0-9])$");
	if (std::regex_match(ip, ip_addr))
	{
		setHost(ip);
		it++;
		checkConfigValues(directive, it);
	}
	else
		throw std::runtime_error("host: invalid ip address");
}

void	ServerSettings::parsePort(std::vector<std::string>& directive, std::vector<std::string>::iterator& it) 
{
	ConfigUtilities::checkDuplicates(port, "port:");
	ConfigUtilities::checkVectorEnd(directive, it, "port: empty value");
	ConfigUtilities::checkSemiColon(directive, it, "port: syntax error");
	int port_num;
	try {
		port_num = stoi(*it);
	} catch(std::exception& e) {
		throw std::invalid_argument("port: (nan)");
	}
	if (port_num >= 0 && port_num < 65535)
	{
		port = port_num;
		it++;
		checkConfigValues(directive, it);
	}
	else
		throw std::runtime_error("invalid port number/invalid syntax");
}

void	ServerSettings::parseMaxBodySize(std::vector<std::string>& directive, std::vector<std::string>::iterator& it) 
{
	ConfigUtilities::checkDuplicates(max_client_body_size, "max client body:");
	ConfigUtilities::checkVectorEnd(directive, it, "Max_body_size: empty value");
	ConfigUtilities::checkSemiColon(directive, it, "Max_body_size: syntax error");
	int size;
	try {
		size = stoi(*it);
	} catch(std::exception& e) {
		throw std::invalid_argument("Max_body_size: (nan)");
	}
	if (size > 0 && size < 9437184)
	{
		max_client_body_size = size;
		it++;
		checkConfigValues(directive, it);
	}
	else
		throw std::runtime_error("invalid client max body size/invalid syntax");
}

void	ServerSettings::parseServerNames(std::vector<std::string>& directive, std::vector<std::string>::iterator& it, bool *dup)
{
	ConfigUtilities::checkDuplicates(*dup, "server_names:");
	ConfigUtilities::checkVectorEnd(directive, it, "server_names: empty value");
	std::regex name("(^(www\\.)?([a-zA-Z0-9-]+\\.)+[a-zA-Z]{2,}$)");
	for (; it != directive.end(); it++)
	{
		if (it->compare(";") == 0)
			break ;
		if (std::regex_match(*it, name))
		{
			*dup = true;
			addServerName(*it);
		}
		else
			throw std::runtime_error("server_names: invalid server name/syntax error");
	}
	ConfigUtilities::checkSemiColon(directive, std::prev(it), "server_names: syntax error semi");
	if (std::next(it) != directive.end())
		it++;
	checkConfigValues(directive, it);
}

void	ServerSettings::parseErrorPages(std::vector<std::string>& directive, std::vector<std::string>::iterator& it)  
{
	ConfigUtilities::checkVectorEnd(directive, it, "error_pages: empty error code value");
	ConfigUtilities::checkVectorEnd(directive, it, "error_pages: empty error page value");
	int error_code;
	try {
		error_code = stoi(*(it - 1));
	} catch(std::exception& e) {
		throw std::invalid_argument("error_pages: (nan)");
	}
	if (error_code > 400 && error_code <= 505)
	{
		ConfigUtilities::checkSemiColon(directive, it, "error_pages: syntax error");
		addErrorPage(error_code, *it);
	}
	else
		throw std::runtime_error("error_pages: invalid error code");
	ConfigUtilities::checkVectorEnd(directive, it, "error_pages: syntax error");
	checkConfigValues(directive, it);
}


void	ServerSettings::parseLocationBlockSettings(std::vector<std::string>& location, std::vector<std::string>::iterator& it)
{
	
	auto it2 = locations.insert({*it, LocationSettings(*it)});
	if (!it2.second)
		throw std::runtime_error("location: duplicate URI");
	std::string directives[9] = {"root", "index", "methods", "autoindex", "redirect", "error_page", "cgi_path", "cgi_script", "upload"};
	std::string	key = *it;
	bool		isValid = false;
	ConfigUtilities::checkVectorEnd(location, it, "location: invalid syntax");
	ConfigUtilities::shiftLocationBlock(location, it);
	for (;*it != "}"; it++)
	{
		isValid = false;
		for (int i = 0; i < 9; i++)
		{
			if (!it->compare(directives[i]))
			{
				isValid = true;
				switch(i + 1)
				{
					case 1:
						locations[key].parseRoot(location, it);
						break;
					case 2:
						locations[key].parseDefaultFile(location, it);
						break;
					case 3:
						locations[key].parseMethods(location, it);
						break;
					case 4:
						locations[key].parseAutoIndex(location, it);
						break;
					case 5:
						locations[key].parseRedirect(location, it);
						break;
					case 6:
						locations[key].parseLocationErrorPages(location, it);
						break;
					case 7:
						locations[key].parseCgiPath(location, it);
						break;
					case 8:
						locations[key].parseCgiScript(location, it);
						break;
					case 9:
						locations[key].parseCgiUpload(location, it);
						break;
					default:
						throw std::logic_error("Something went very wrong...");
				}
				break ;
			}
		}
		if (!isValid)
			throw std::runtime_error("location: invalid directive");
	}
	ConfigUtilities::printLocationBlock(locations[key]);
}

void	ServerSettings::parseLocationBlock(std::vector<std::string>& location, std::vector<std::string>::iterator& it)
{
	
	ConfigUtilities::checkVectorEnd(location, it, "location: invalid URI/path");
	std::regex uri("\\/[a-zA-Z0-9._\\-\\/]*");
	if (std::next(it) != location.end() && std::regex_match(*it, uri) && !std::next(it)->compare("{"))
		parseLocationBlockSettings(location, it);
	else
		throw std::runtime_error("location: syntax error");
}
