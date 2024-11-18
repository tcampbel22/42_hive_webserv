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
#include "../include/GlobalSettings.hpp"
#include <vector>
#include <unordered_map>
#include <variant>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#include <fstream>

class ConfigParser
{
private:
	std::string	configFileStr;
	// std::vector<LocationSettings> locations;
	GlobalSettings global;
public:
	ConfigParser();
	ConfigParser(std::string file);
	~ConfigParser();
	void		readConfigFile(std::string);
	void		parseConfigFile();
	void		initialParse();
	void		removeComments();
	template<typename T>
	T	getGlobal(std::string key);
	std::string	getConfigFileStr();
	
};

template<typename T>
T	ConfigParser::getGlobal(std::string key) 
{
	return global.getGlobalSetting<T>(key); 
}