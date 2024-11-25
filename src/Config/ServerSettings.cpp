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
	host =  "";
	port = -1;
	max_client_body_size =  -1; //megabytes
	addServerName("www.example.com");
	addErrorPage(404, "/root/var/html/404.html");
	addErrorPage(500, "/root/var/html/404.html");
}

ServerSettings::~ServerSettings() {}


void	ServerSettings::parseServerBlock(std::vector<std::string> serverBlock)
{
	ConfigUtilities::checkBrackets(serverBlock);
	// ConfigUtilities::trimServerBlock(serverBlock);
	//check if the server is first and set as default
	const std::string	directives[6] = {"host", "port", "server_names", "client_max_body_size", "location", "error_pages"};
	int					pos = 0;
	
	for (auto it = serverBlock.begin() + 2; it != serverBlock.end() - 1; it++)
	{
		for (int i = 0; i < 6; i++)
		{
			if (it->compare(directives[i]) == 0)
			{
				pos = i + 1;
				std::cout << pos << '\n';
				switch (pos)
				{
				case 1: //host
					parseHost(it);
					break;
				case 2: //port
					parsePort(it);
					break;
				case 3: //server_names
					//parseServerNames(it);
					break;
				case 4: //client_max_body
					parseMaxBodySize(it);
					break;
				case 5: //location block
					//parseLocationBlock(it);
					// cycleLocations(*it);
					break;
				case 6: //error page
					//parseErrorPages(it);
					break;
				default:
					break;
				}
			}
			else
				throw std::runtime_error("invalid directive in configuration file");
		}
		//check host
		//check port
		//check servernames
		//check error pages -> 400, 401, 403, 404, 500, 502, 503
		//check client body size
		//Check for duplicates, missing semi colons, uneven brackets, non directives 
		//call locations cycle locations within current vector
	}
}



void	ServerSettings::parseHost(std::vector<std::string>::iterator& it) 
{
	if (!getHost().empty())
		throw std::runtime_error("duplicate host directive");
	std::string ip = *(it + 1);
	std::regex ip_addr("^((25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]?[0-9])\\.){3}(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]?[0-9])$");
	if (std::regex_match(ip, ip_addr) && (it + 2)->compare(";") == 0)
	{
		setHost(ip);
		it += 3;
	}
	else
		throw std::runtime_error("invalid ip address");
}

void	ServerSettings::parsePort(std::vector<std::string>::iterator& it) 
{
	if (getPort() != -1)
		throw std::runtime_error("duplicate port directive");
	std::string port = *(it + 1);
	int port_num = stoi(port);
	if (port_num > 0 && port_num < 65535 && (it + 2)->compare(";") == 0)
	{
		setPort(port_num);
		it += 3;
	}
	else
		throw std::runtime_error("invalid port number/invalid syntax");
}

void	ServerSettings::parseMaxBodySize(std::vector<std::string>::iterator& it) 
{
	if (getMaxClientBody() != -1)
		throw std::runtime_error("duplicate max client body directive");
	std::string str = *(it + 1);
	int size = stoi(str);
	if (size > 0 && size < 9437184 && (it + 2)->compare(";") == 0)
	{
		setMaxClientBodySize(size);
		it += 3;
	}
	else
		throw std::runtime_error("invalid client max body size/invalid syntax");
}

// void	ServerSettings::parseServerNames(std::vector<std::string>::iterator& it) {}
// void	ServerSettings::parseLocationBlock(std::vector<std::string>::iterator& it) {}
// void	ServerSettings::parseErrorPages(std::vector<std::string>::iterator& it)  {}


void		ServerSettings::parseServerSettings(std::vector<std::string> tokens) 
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


//GETTERS

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
std::vector<int>	ServerSettings::getLocationMethods(std::string key)
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

// std::unordered_map<std::string, LocationSettings> ServerSettings::getLocationSettings() { return locations; }

//SETTERS

void	ServerSettings::addServerName(std::string name) { server_names.push_back(name); }
void	ServerSettings::addErrorPage(int status, std::string path) { error_pages[status].push_back(path); }
void	ServerSettings::setHost(std::string ip) { host = ip; }
void	ServerSettings::setPort(int port_num) { port = port_num; }
void	ServerSettings::setMaxClientBodySize(int size) { max_client_body_size = size; }
