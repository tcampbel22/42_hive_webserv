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

#include "../include/ConfigParser.hpp"

ConfigParser::ConfigParser() {}

ConfigParser::ConfigParser(std::string file) 
{
	readConfigFile(file);
}

int	checkConfName(std::string file)
{
	int pos = file.find('.');
	std::string suffix = file.substr(pos, file.length());
	if (suffix.compare(".conf"))
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
		std::cout << "reverting to default server\n...\n";
		sleep(1);
		std::cout << "default server configured\n";
		return ;
	}
	infile.open(file);
	if (!infile.is_open())
	{
		std::cout << "failed to open config file\n";
		std::cout << "reverting to default server\n...\n";
		sleep(1);
		std::cout << "default server configured\n";
		return ;
	}
	stream << infile.rdbuf();
	configFileStr = stream.str();
	infile.close();
	
}

void		ConfigParser::parseConfigFile() 
{
	initialParse();
	global.parseGlobalSettings(configFileStr);
	// locations.parseLocations(configFileStr);
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

void	ConfigParser::initialParse()
{
	removeComments();
}

std::string	ConfigParser::getConfigFileStr() { return configFileStr; }

ConfigParser::~ConfigParser() {}