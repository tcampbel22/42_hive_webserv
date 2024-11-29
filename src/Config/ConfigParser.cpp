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

#include "ConfigParser.hpp"

ConfigParser::ConfigParser() {}

ConfigParser::ConfigParser(std::string file)
{
	readConfigFile(file);
}

int	checkConfName(std::string file)
{
	int pos = file.find('.');
	std::string suffix = file.substr(pos, file.length() - pos);
	if (suffix.compare(".conf") != 0)
		return 1;
	return 0;
}
void	ConfigParser::readConfigFile(std::string file)
{
	std::fstream		infile;
	std::ostringstream	stream;

	if (checkConfName(file))
	{
		std::cout << "invalid config file\n";
		exit(1) ;
	}
	infile.open(file);
	if (!infile.is_open())
	{
		std::cout << "failed to open config file\n";
		exit(1) ;
	}
	stream << infile.rdbuf();
	configFileStr = stream.str();
	infile.close();
}

void		ConfigParser::parseConfigFile() 
{
	initialParse();
	splitServerBlocks();
	 //need to check how many servers there are, then create that many instances
	// for (int i = 0; i < server_count; i++)
	// 	settings.push_back(ServerSettings());
	// settings[0].parseServerSettings(tokens);
}

void	ConfigParser::removeComments()
{
	int 	i = -1;
	uint	start = 0;
	while (configFileStr[++i] != '\0')
	{	
		if (configFileStr[i] == '#')
		{
			start = i;
			while (configFileStr[i] != '\n')
				i++;
			configFileStr.erase(start, i - start);
			i = start;
		}
	}
}

void	ConfigParser::countServers() 
{
	std::regex servers("server\\s+[^\\{]*\\{[^\\}]");
	std::sregex_iterator begin(configFileStr.begin(), configFileStr.end(), servers);
	std::sregex_iterator end;
	server_count = std::distance(begin, end);
}

void	ConfigParser::tokenise(const std::string& c)
{
	std::string token;

	for (size_t i = 0; i < c.length(); i++)
	{
		if (isspace(c[i]))
		{
			if (!token.empty())
			{
				tokens.push_back(token);
				token.clear();
			}
		}
		else if (c[i] == '{' || c[i] == '}' || c[i] == ';')
		{
			if (!token.empty())
			{
				tokens.push_back(token);
				token.clear();
			}
			if (!tokens.empty() && tokens.back().find_first_of("{;") != std::string::npos && tokens.back().back() == c[i])
				tokens.back().push_back(c[i]);
			else
				tokens.push_back(std::string(1, c[i]));
		}
		else
			token.push_back(c[i]);
	}
	if (!token.empty())
		tokens.push_back(token);
}

void	ConfigParser::initialParse()
{
	removeComments();
	// countServers();
	tokenise(configFileStr);
}

std::string	ConfigParser::getConfigFileStr() { return configFileStr; }

void	createKey(std::vector<std::string>::iterator start,  std::vector<std::string>::iterator end, std::string& key)
{
	std::string	_host;
	std::string	_port;
	auto host_it = std::find(start, end, "host");
	if (host_it->compare("host"))
		throw std::runtime_error("no host in server block");
	if (std::next(host_it) != end)
		_host = *(host_it + 1);
	auto port_it = std::find(start, end, "port");
	if (port_it->compare("port"))
		throw std::runtime_error("no port in server block");
	if (std::next(port_it) != end)
		_port = *(port_it + 1);
	key = _host + ":" + _port;
	std::cout << key << '\n';
	
}

void	ConfigParser::splitServerBlocks() 
{
	if (tokens.begin()->compare("server") && tokens[1] != "{")
		throw std::runtime_error("Configuration file should start with server block");
	auto it = tokens.begin();
	for (; it != tokens.end(); it++)
	{
		if (!it->compare("server") && !std::next(it)->compare("{"))
		{
			auto start = it + 2;
			std::string key;
			for (; it != tokens.end(); it++)
			{
				if (!it->compare("}") && (!std::next(it)->compare("server") || std::next(it) == tokens.end()))
				{
					createKey(start, it, key);
					auto dup = settings.insert({key, ServerSettings()});
					if (!dup.second)
						throw std::runtime_error("config: duplicate host/port");
					//settings[key].parseServerSettings(tokens); //PRIORITY
					break ;
				}
			} 
		}
	}
}

ConfigParser::~ConfigParser() {}