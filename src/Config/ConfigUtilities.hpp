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

#include <vector>
#include "ConfigParser.hpp"
#include "ServerSettings.hpp"
#include "LocationSettings.hpp"

class ConfigUtilities
{
public:
	ConfigUtilities();
	~ConfigUtilities();
	static	void	checkBrackets(std::vector<std::string> serverBlock);
	static	void	trimServerBlock(std::vector<std::string>& serverBlock);
};
