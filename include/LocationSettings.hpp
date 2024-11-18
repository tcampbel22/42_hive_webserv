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

#include <unordered_map>
#include <iostream>
#include <variant>
#include <vector>

class LocationSettings
{
private:
	std::unordered_map<std::string, std::variant<std::string, bool, uint, std::vector<std::string>>> location_settings;
public:
	LocationSettings();
	~LocationSettings();
};