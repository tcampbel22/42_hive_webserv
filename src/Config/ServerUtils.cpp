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

//GETTERS

bool						ServerSettings::isDefaultServer() { return _isDefaultServer; }
int							ServerSettings::getPort() { return port; }
std::string&				ServerSettings::getHost() { return host; }
std::vector<std::string>&	ServerSettings::getServerNames() { return server_names; } //retuns vector containing all server names
int							ServerSettings::getMaxClientBody() { return max_client_body_size; } //in bytes
std::string					ServerSettings::getKey() { return _key; }
std::unordered_map<int, std::vector<std::string>>&	ServerSettings::getAllErrorPages() { return error_pages; }

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

void	ServerSettings::addServerName(std::string name) { server_names.push_back(name); } //Need to check for duplicates
void	ServerSettings::setHost(std::string ip) { host = ip; }
void	ServerSettings::setPort(int port_num) { port = port_num; }
void	ServerSettings::setMaxClientBodySize(int size) { max_client_body_size = size; }
void	ServerSettings::setDefaultServer(bool val) { _isDefaultServer = val; }

void	ServerSettings::addErrorPage(int status, std::string path) 
{ 
	if (error_pages.find(status) != error_pages.end())
	{
		if (std::find(error_pages[status].begin(), error_pages[status].end(), path) != error_pages[status].end())
			throw std::runtime_error("error_pages: duplicate path");
	}
	error_pages[status].push_back(path);
}



void	ServerSettings::checkConfigValues(std::vector<std::string>& directive, std::vector<std::string>::iterator& it)
{
	if (it == directive.end() || std::next(it) == directive.end())
	{
		if (port == -1 || host.empty())
			throw std::runtime_error("host or port missing from configuration file");
		if (locations.find("/") == locations.end())
			throw std::runtime_error("root directory missing from configuration file");
		if (max_client_body_size == -1)
			max_client_body_size = MAX_BODY_SIZE;
	}
}