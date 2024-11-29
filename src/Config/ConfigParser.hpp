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

#include "../../include/webserv.hpp"
#include "ServerSettings.hpp"
#include "LocationSettings.hpp"
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <cctype>

class ServerSettings;

class ConfigParser
{
private:
	std::string					configFileStr;
	std::vector<std::string>	tokens; 
	int							server_count;
public:
	std::unordered_map<std::string, ServerSettings> settings;
	ConfigParser();
	ConfigParser(std::string file);
	~ConfigParser();
	void		readConfigFile(std::string);
	void		parseConfigFile();
	void		initialParse();
	void		removeComments();
	std::string	getConfigFileStr();
	void		countServers();
	void		tokenise(const std::string& config);
	void		splitServerBlocks();
};
