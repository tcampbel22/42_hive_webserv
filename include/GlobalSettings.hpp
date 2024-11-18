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
#include <vector>
#include <unordered_map>
#include <variant>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <regex>

class GlobalSettings
{
private:
	std::unordered_map<std::string, std::variant<std::string, uint, int, std::vector<std::string>, std::unordered_map<uint, std::vector<std::string>>>> global_map;
	std::unordered_map<uint, std::vector<std::string>> error_pages;
	std::vector<std::regex> regexList;
	// this map would all the global settings host, port, server_name, max_client_body, error message html (html error message would be stored inside ) etc

public:
	GlobalSettings();
	~GlobalSettings();
	void		parseGlobalSettings(std::string configFile);
	void		addGlobalSetting(std::string key, std::variant<std::string, uint, int, std::vector<std::string>, std::unordered_map<uint, std::vector<std::string>>> value);
	void		addErrorPage(uint status, std::string path);
	template<typename T>
	T			getGlobalSetting(std::string key);
	// uint		getPort();
	// std::string	getHost();
	// std::string getServerName();
	// uint		getMaxClientBody();
	// std::vector<std::string> get404Error();
	// std::vector<std::string> get500Error();
};

template<typename T>
T	GlobalSettings::getGlobalSetting(std::string key)
{
	auto it = global_map.find(key);
	if (it != global_map.end())
	{
		if (std::holds_alternative<T>(it->second))
			return std::get<T>(it->second); 
	}
	else
		ft_perror(key + "key not found");
	return (0);
}	