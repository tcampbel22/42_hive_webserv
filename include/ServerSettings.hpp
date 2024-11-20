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
#include "../include/LocationSettings.hpp"
#include <vector>
#include <unordered_map>
#include <variant>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <regex>
#include <string_view>

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
	void						parseServerSettings(std::string config);
	void						parseLocationSettings(std::string_view location);
	void						addErrorPage(int status, std::string path);
	void						addServerName(std::string name);
	void						setLocationSettings(const std::string& key);
	bool						isDefaultServer();
	int							getPort();
	std::string					getHost();
	std::vector<std::string> 	getServerNames();
	int							getMaxClientBody();
	std::vector<std::string>	getErrorPages(int key);
	std::string 				getLocationPath(std::string key);
	std::string 				getLocationRoot(std::string key);
	std::string 				getLocationDefaultFile(std::string key);
	bool 						getLocationAutoIndex(std::string key);
	std::vector<std::string>	getLocationMethods(std::string key);
	std::unordered_map<std::string, LocationSettings> getLocationSettings();
};