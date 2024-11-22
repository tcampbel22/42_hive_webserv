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
	std::string suffix = file.substr(pos, file.length());
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
		return ;
	}
	infile.open(file);
	if (!infile.is_open())
	{
		std::cout << "failed to open config file\n";
		return ;
	}
	stream << infile.rdbuf();
	configFileStr = stream.str();
	infile.close();
}

void		ConfigParser::parseConfigFile() 
{
	initialParse(); //need to check how many servers there are, then create that many instances
	for (int i = 0; i < server_count; i++)
		settings.push_back(ServerSettings());
	settings[0].parseServerSettings(tokens);
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

void	ConfigParser::tokenise(const std::string& config)
{
	std::string token;

	for (char c : config)
	{
		if (isspace(c) || c == '{' || c == '}' || c == ';')
		{
			if (!token.empty())
			{
				tokens.push_back(token);
				token.clear();
			}
		if (c == '{' || c == '}' || c == ';')
			tokens.push_back(std::string(1, c));
		}
		else
			token += c;
	}
	if (!token.empty())
		tokens.push_back(token);
}

void	ConfigParser::initialParse()
{
	removeComments();
	countServers();
	tokenise(configFileStr);
	for (auto it = tokens.begin(); it != tokens.end(); it++)
		std::cout << *it << '\n';
}

std::string	ConfigParser::getConfigFileStr() { return configFileStr; }

ConfigParser::~ConfigParser() {}