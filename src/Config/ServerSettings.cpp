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

ServerSettings::ServerSettings() : isDefault(false), host(""), port(-1), max_client_body_size(-1)
{
	addServerName("www.example.com");
}

ServerSettings::~ServerSettings() {}


void	ServerSettings::parseServerBlock(std::vector<std::string>& serverBlock)
{
	ConfigUtilities::checkBrackets(serverBlock);
	ConfigUtilities::trimServerBlock(serverBlock);
	//check if the server is first and set as default
	// locations.insert({"/", LocationSettings(1)});
	// locations.insert({"hello", LocationSettings(2)});
	// locations.insert({"there", LocationSettings(3)});
	const std::string	directives[6] = {"host", "port", "client_max_body_size", "server_names", "location", "error_page"};
	bool				isValid = false;
	bool				dupServerName = false;
	
	for (auto it = serverBlock.begin(); it != serverBlock.end(); it++)
	{
		isValid = false;
		for (int i = 0; i < 6; i++)
		{
			if (it->compare(directives[i]) == 0)
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
					// cycleLocations(*it);
					break;
				case 6: //error page
					parseErrorPages(serverBlock, it);
					break;
				default:
					throw std::logic_error("Something went very wrong....");
				}
				break ;
			}
		}
	}
	if (!isValid)
		throw std::runtime_error("invalid directive in configuration file");
}
//check host
//check port
//check servernames
//check error pages -> 400, 401, 403, 404, 500, 502, 503
//check client body size
//Check for duplicates, missing semi colons, uneven brackets, non directives 
//call locations cycle locations within current vector



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
		std::cout << "Host: " << host << '\n';
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
		std::cout << "Port: " << port << '\n';
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
		std::cout << "Max: " << getMaxClientBody() << '\n';
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
			std::cout << "Server name: " << *it << '\n';
		}
		else
			throw std::runtime_error("server_names: invalid server name/syntax error");
	}
	ConfigUtilities::checkSemiColon(directive, std::prev(it), "server_names: syntax error semi");
	ConfigUtilities::checkVectorEnd(directive, it, "server_names: syntax error");
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
		std::cout << "Error code: " <<*(it - 1) << " Path: " << *it << '\n';
	}
	else
		throw std::runtime_error("error_pages: invalid error code");
	checkConfigValues(directive, it);
}


void		ServerSettings::parseServerSettings(std::vector<std::string>& tokens) 
{
	if (tokens.begin()->compare("server"))
		throw std::runtime_error("Configuration file should start with server block");
	if (tokens[1] != "{")
		throw std::runtime_error("Server block missing curly bracket");
	//splitServerBlocks() //Need to create map of vectors of server blocks
	// for (auto it = tokens.begin(); it != tokens.end(); it++)
	// {
	parseServerBlock(tokens); //needs to send in each server block individually
	// }		
}

void	ServerSettings::parseLocationBlockSettings(std::vector<std::string>& location, std::vector<std::string>::iterator& it)
{
	
	auto it2 = locations.insert({*it, LocationSettings(*it)});
	if (!it2.second)
		throw std::runtime_error("location: duplicate URI");
	std::string directives[5] = {"root", "index", "methods", "autoindex", "redirect"};
	bool		isValid = false;
	// bool		dupMethod = false;
	ConfigUtilities::shiftLocationBlock(location, it);
	for (;*it != "}"; it++)
	{
		for (int i = 0; i < 6; i++)
		{
			if (!it->compare(directives[i]))
			{
				isValid = true;
				switch(i + 1)
				{
					case 1:
						locations[*it].parseRoot(location, it);
						break;
					case 2:
						// locations[*it].parseMethods(location, it);
						// dupMethod = true;
						break;
					case 3:
						// locations[*it].parseRedirect(location, it);
						break;
					case 4:
						locations[*it].parseDefaultFile(location, it);
						break;
					case 5:
						// locations[*it].parseAutoIndex(location, it);
						break;
					default:
						throw std::logic_error("Something went very wrong...");
					break ;
				}
			}

		}
		if (!isValid)
			throw std::runtime_error("location: invalid directive");
	}
}

void	ServerSettings::parseLocationBlock(std::vector<std::string>& location, std::vector<std::string>::iterator& it)
{
	
	ConfigUtilities::checkVectorEnd(location, it, "location: invalid URI/path");
	std::regex uri("\\/[a-zA-Z0-9._\\-\\/]*");
	if (std::next(it) != location.end() && std::regex_match(*it, uri) && std::next(it)->compare("{"))
		parseLocationBlockSettings(location, it);
	else
		throw std::runtime_error("location: syntax error here");
}

//GETTERS

bool			ServerSettings::isDefaultServer() { return isDefault; }
int				ServerSettings::getPort() { return port; }
std::string&	ServerSettings::getHost() { return host; }
std::vector<std::string>&	ServerSettings::getServerNames() { return server_names; } //retuns vector containing all server names
int				ServerSettings::getMaxClientBody() {return max_client_body_size; } //in megabytes

std::vector<std::string>& ServerSettings::getErrorPages(int status) 
{
	if (error_pages.find(status) != error_pages.end())
	{
		auto it = error_pages.find(status);
		return it->second;
	}
	else 
		throw std::runtime_error("status not found");
}

std::string&	ServerSettings::getLocationPath(std::string key) 
{
	auto it = locations.find(key);
	if (it != locations.end())
		return locations[key].getPath();
	else
		throw std::runtime_error(key + ": key not found in location setting block");
}

std::string&	ServerSettings::getLocationRoot(std::string key) 
{
	auto it = locations.find(key);
	if (it != locations.end())
		return locations[key].getRoot();
	else
		throw std::runtime_error(key + ": key not found in location setting block");
}

std::string&	ServerSettings::getLocationDefaultFile(std::string key)
{
	auto it = locations.find(key);
	if (it != locations.end())
		return locations[key].getDefaultFile();
	else
		throw std::runtime_error(key + ": key not found in location setting block");
}

bool	ServerSettings::getLocationAutoIndex(std::string key)
{
	auto it = locations.find(key);
	if (it != locations.end())
		return locations[key].isAutoIndex();
	else
	{
		ft_perror(key +": not found");
		return false;
	}
}
std::vector<int>&	ServerSettings::getLocationMethods(std::string key)
{
	auto it = locations.find(key);
	if (it != locations.end())
		return locations[key].getMethods();
	else
		throw std::runtime_error(key + "not found in location block");
}

LocationSettings*			ServerSettings::getLocationBlock(const std::string key)
{
	auto it = locations.find(key);
	if (it != locations.end())
		return &(it->second);
	else
		return nullptr;
}

//SETTERS

void	ServerSettings::addServerName(std::string name) { server_names.push_back(name); }
void	ServerSettings::addErrorPage(int status, std::string path) 
{ 
	if (error_pages.find(status) != error_pages.end())
	{
		if (std::find(error_pages[status].begin(), error_pages[status].end(), path) != error_pages[status].end())
			throw std::runtime_error("error_pages: duplicate path");
	}
	error_pages[status].push_back(path);
}

void	ServerSettings::setHost(std::string ip) { host = ip; }
void	ServerSettings::setPort(int port_num) { port = port_num; }
void	ServerSettings::setMaxClientBodySize(int size) { max_client_body_size = size; }


void	ServerSettings::checkConfigValues(std::vector<std::string>& directive, std::vector<std::string>::iterator& it)
{
	if (std::next(it) == directive.end())
	{
		if (port == -1 || host.empty() || max_client_body_size == -1)
			throw std::runtime_error("values missing from configuration file");
	}
}