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
#include "ConfigUtilities.hpp"
#include "LocationSettings.hpp"
#include <vector>
#include <unordered_map>
#include <variant>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <regex>
#include <iterator>

#define GET 1
#define POST 2
#define DELETE 3

class LocationSettings;

class ServerSettings
{
private:
	bool		isDefault;
	std::string host;
	int			port;
	int			max_client_body_size;
	std::unordered_map<int, std::vector<std::string>> error_pages;
	std::vector<std::string> server_names;
	std::unordered_map<std::string, LocationSettings> locations;

public:
	ServerSettings();
	~ServerSettings();
	//PARSERS
	void						parseServerSettings(std::vector<std::string>& tokens);
	void						parseServerBlock(std::vector<std::string>& serverBlock);
	void						parseHost(std::vector<std::string>& directive, std::vector<std::string>::iterator& it);
	void						parsePort(std::vector<std::string>& directive, std::vector<std::string>::iterator& it);
	void						parseServerNames(std::vector<std::string>& directive, std::vector<std::string>::iterator& it, bool *dup);
	void						parseMaxBodySize(std::vector<std::string>& directive, std::vector<std::string>::iterator& it);
	void						parseErrorPages(std::vector<std::string>& serverBlock, std::vector<std::string>::iterator& it);
	void						checkConfigValues(std::vector<std::string>& directive, std::vector<std::string>::iterator& it);
	void						parseLocationBlock(std::vector<std::string>& location, std::vector<std::string>::iterator& it);
	void						parseLocationBlockSettings(std::vector<std::string>& location, std::vector<std::string>::iterator& it);
	
	//SETTERS
	void						setHost(std::string ip);
	void						setPort(int port_num);
	void						setMaxClientBodySize(int size);
	void						addErrorPage(int status, std::string path);
	void						addServerName(std::string name);
	void						setLocationSettings(const std::string& key);
	//GETTERS
	bool						isDefaultServer();
	int							getPort();
	std::string&				getHost();
	std::vector<std::string>& 	getServerNames();
	int							getMaxClientBody();
	std::vector<std::string>&	getErrorPages(int key);
	std::string& 				getLocationPath(std::string key);
	std::string& 				getLocationRoot(std::string key);
	std::string& 				getLocationDefaultFile(std::string key);
	bool 						getLocationAutoIndex(std::string key);
	std::vector<int>&			getLocationMethods(std::string key);
	LocationSettings*			getLocationBlock(const std::string key);
	// std::unordered_map<std::string, LocationSettings> getLocationSettings();
};