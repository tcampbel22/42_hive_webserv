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

ServerSettings::ServerSettings()
{
	isDefault = false;
	host =  "127.0.0.1";
	port = 8000;
	max_client_body_size =  5;
	addServerName("www.example.com");
	addErrorPage(404, "/root/var/html/404.html");
	addErrorPage(500, "/root/var/html/404.html");
}

ServerSettings::~ServerSettings() {}


void	ServerSettings::parseServerBlock(std::vector<std::string> serverBlock)
{
	for (auto it = serverBlock.begin(); it != serverBlock.end(); it++)
	{
		std::string directives[6] = {"host", "port", "server_names", "client_max_body_size", "location", "error_pages"};
		//check host
		//check port
		//check servernames
		//check error pages
		//check client body size
		//Check for duplicates, missing semi colons, uneven brackets, non directives 
		//call locations cycle locations within current vector
	}
}

void		ServerSettings::parseServerSettings(std::vector<std::string> tokens) 
{
	if (tokens.begin()->compare("server"))
		throw std::runtime_error("Configuration file should start with server block");
	//splitServerBlocks() //Need to create map of vectors of server blocks
	for (auto it = tokens.begin(); it != tokens.end(); it++)
	{
		// parseServerBlock(serverBlock);
	}		
	// cycleLocations(config);
}

void	ServerSettings::parseLocationSettings(std::string_view location)
{
	int start = location.find_first_of('/');
	int end = location.find_first_of('{') - 1;
	const std::string key(location.substr(start, end - start));
	setLocationSettings(key);
}

void	ServerSettings::cycleLocations(std::string config)
{
	std::regex location("location\\s+[^\\{]*\\{[^\\}]*\\}");
	std::smatch match;
	std::sregex_iterator begin(config.begin(), config.end(), location);
	std::sregex_iterator end;
	for (std::sregex_iterator it = begin; it != end; it++)
	{
		match = *it;
		std::string_view matched_string(config.data() + match.position(), match.length());
		parseLocationSettings(matched_string);
	}
}

void	ServerSettings::setLocationSettings(const std::string& key)
{
	auto it = locations.insert({key, LocationSettings(key)});
	if (!it.second)
		throw std::runtime_error("Duplicate key " + key);
}
void	ServerSettings::addServerName(std::string name)
{
	server_names.push_back(name);
}

void	ServerSettings::addErrorPage(int status, std::string path)
{
	error_pages[status].push_back(path);
}


bool		ServerSettings::isDefaultServer() { return isDefault; }

int			ServerSettings::getPort() { return port; }

std::string	ServerSettings::getHost() { return host; }

std::vector<std::string> ServerSettings::getServerNames() { return server_names; } //retuns vector containing all server names

int		ServerSettings::getMaxClientBody() {return max_client_body_size; } //in megabytes

std::vector<std::string> ServerSettings::getErrorPages(int status) 
{
	if (error_pages.find(status) != error_pages.end())
	{
		auto it = error_pages.find(status);
		return it->second;
	}
	else 
		throw std::runtime_error("status not found");
}

std::string	ServerSettings::getLocationPath(std::string key) 
{
	auto it = locations.find(key);
	if (it != locations.end())
		return locations[key].getPath();
	else
		return "key not found";
}

std::string	ServerSettings::getLocationRoot(std::string key) 
{
	auto it = locations.find(key);
	if (it != locations.end())
		return locations[key].getRoot();
	else
	return "key not found";
}
std::string	ServerSettings::getLocationDefaultFile(std::string key)
{
	auto it = locations.find(key);
	if (it != locations.end())
		return locations[key].getDefaultFilePath();
	else
		return "key not found";
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
std::vector<std::string>	ServerSettings::getLocationMethods(std::string key)
{
	auto it = locations.find(key);
	if (it != locations.end())
		return locations[key].getMethods();
	else
		return locations[key].getMethods();
}

LocationSettings*			ServerSettings::getLocationBlock(const std::string key)
{
	auto it = locations.find(key);
	if (it != locations.end())
		return &(it->second);
	else
		return nullptr;
}

std::unordered_map<std::string, LocationSettings> ServerSettings::getLocationSettings() { return locations; }