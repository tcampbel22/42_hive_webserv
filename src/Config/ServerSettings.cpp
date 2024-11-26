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
	locations.insert({"/", LocationSettings(1)});
	locations.insert({"hello", LocationSettings(2)});
	locations.insert({"there", LocationSettings(3)});
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
				case 3: 
					parseMaxBodySize(serverBlock, it);
					break;
				case 4: //server_names
					parseServerNames(serverBlock, it, &dupServerName);
					break;
				case 5: //location block
					// parseLocationBlock(it);
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
	if (!host.empty())
		throw std::runtime_error("host: duplicate directive");
	if (std::next(it) == directive.end())
		throw std::runtime_error("host: empty value");
	it++;
	if (std::next(it) == directive.end() || (it + 1)->compare(";"))
		throw std::invalid_argument("host: syntax error");
	std::string ip = *it;
	std::regex ip_addr("^((25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]?[0-9])\\.){3}(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]?[0-9])$");
	if (std::regex_match(ip, ip_addr))
	{
		setHost(ip);
		it++;
		// std::cout << "Host: " << host << '\n';
		if (std::next(it) == directive.end())
			checkConfigValues();
	}
	else
		throw std::runtime_error("host: invalid ip address");
}

void	ServerSettings::parsePort(std::vector<std::string>& directive, std::vector<std::string>::iterator& it) 
{
	if (port != -1)
		throw std::runtime_error("duplicate port directive");
	if (std::next(it) == directive.end())
		throw std::runtime_error("port: empty value");
	it++;
	if (std::next(it) == directive.end() || (it + 1)->compare(";"))
		throw std::invalid_argument("port: syntax error");
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
		// std::cout << "Port: " << port << '\n';
		if (std::next(it) == directive.end())
			checkConfigValues();
	}
	else
		throw std::runtime_error("invalid port number/invalid syntax");
}

void	ServerSettings::parseMaxBodySize(std::vector<std::string>& directive, std::vector<std::string>::iterator& it) 
{
	if (max_client_body_size != -1)
		throw std::runtime_error("duplicate max client body directive");
	if (std::next(it) == directive.end())
		throw std::runtime_error("Max_body_size: empty value");
	it++;
	if (std::next(it) == directive.end() || (it + 1)->compare(";"))
		throw std::invalid_argument("Max_body_size: syntax error");
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
		// std::cout << "Max: " << getMaxClientBody() << '\n';
		if (std::next(it) == directive.end())
			checkConfigValues();
	}
	else
		throw std::runtime_error("invalid client max body size/invalid syntax");
}

void	ServerSettings::parseServerNames(std::vector<std::string>& directive, std::vector<std::string>::iterator& it, bool *dup)
{
	if (*dup == true)
		throw std::runtime_error("server_names: duplicate server_name directive");
	if (std::next(it) == directive.end())
		throw std::runtime_error("server_names: empty value");
	it++;
	std::regex name("(^(www\\.)?([a-zA-Z0-9-]+\\.)+[a-zA-Z]{2,}$)");
	for (auto it2 = it; it2 != directive.end(); it2++)
	{
		if (it2->compare(";") == 0)
			break ;
		if (std::regex_match(*it2, name))
		{
			*dup = true;
			addServerName(*it2);
			// std::cout << "Server name: " << *it2 << '\n';
		}
		else
			throw std::runtime_error("server_names: invalid server name/syntax error");
		it = it2;
	}
	if (std::next(it) == directive.end() || (it + 1)->compare(";"))
		throw std::invalid_argument("server_names: syntax error");
	it++;
	if (std::next(it) == directive.end())
		checkConfigValues();
}
// void	ServerSettings::parseLocationBlock(std::vector<std::string>& directive, std::vector<std::string>::iterator& it) {}

void	ServerSettings::parseErrorPages(std::vector<std::string>& directive, std::vector<std::string>::iterator& it)  
{
	if (std::next(it) == directive.end())
		throw std::runtime_error("error_pages: empty error code value");
	it++;
	if (std::next(it) == directive.end())
		throw std::runtime_error("error_pages: empty error page value");
	it++;
	int error_code;
	try {
		error_code = stoi(*(it - 1));
	} catch(std::exception& e) {
		throw std::invalid_argument("error_pages: (nan)");
	}
	if (error_code > 400 && error_code <= 505)
	{
		if (std::next(it) == directive.end() || (it + 1)->compare(";"))
			throw std::runtime_error("error_pages: syntax error");
		addErrorPage(error_code, *it);
		// std::cout << "Error code: " <<*(it - 1) << " Path: " << *it << '\n';
	}
	else
		throw std::runtime_error("invalid client max body size/invalid syntax");
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

// std::unordered_map<std::string, LocationSettings> ServerSettings::getLocationSettings() { return locations; }

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


void	ServerSettings::checkConfigValues()
{
	if (port == -1 || host.empty() || max_client_body_size == -1)
		throw std::runtime_error("values missing from configuration file");
}