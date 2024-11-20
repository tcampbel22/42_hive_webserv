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

#pragma once

#include "../include/webserv.hpp"
#include "../include/ServerSettings.hpp"
#include "../include/LocationSettings.hpp"
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#include <fstream>

class ConfigParser
{
private:
	std::string	configFileStr;
public:
	std::vector<ServerSettings> settings;
	ConfigParser();
	ConfigParser(std::string file);
	~ConfigParser();
	void		readConfigFile(std::string);
	void		parseConfigFile();
	void		initialParse();
	void		removeComments();
	std::string	getConfigFileStr();
	
};
