/************************************************/
/** __          __  _                          **/
/** \ \        / / | |                         **/
/**  \ \  /\  / /__| |__  ___  ___ _ ____   __ **/
/**   \ \/  \/ / _ \ '_ \/ __|/ _ \ '__\ \ / / **/
/**    \  /\  /  __/ |_) \__ \  __/ |   \ V /  **/
/**     \/  \/ \___|_.__/|___/\___|_|    \_/   **/
/**                                            **/
/**                                            **/
/**             W E B S E R V                  **/
/************************************************/

#pragma once

#include <vector>
#include <unordered_map>
#include <variant>
#include <iostream>

class ConfigParser
{
private:
	std::unordered_map<std::string, std::variant<std::string, uint, std::vector<std::string>, std::unordered_map<std::string, std::string>>> server_settings;
	// Unordered map that can hold string vector, string, int and unordered map with string key and string value

public:
	ConfigParser();
	~ConfigParser();
	uint		getPort();
	std::string	getHost();
	std::string getServerName();
	uint		getMaxClientBody();
	std::vector<std::string> get404Error();
	std::vector<std::string> get500Error();
	

};