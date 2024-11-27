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
#include <variant>

class ConfigUtilities
{
public:
	ConfigUtilities();
	~ConfigUtilities();
	static	void	checkBrackets(std::vector<std::string> serverBlock);
	static	void	trimServerBlock(std::vector<std::string>& serverBlock);
	static	void	shiftLocationBlock(std::vector<std::string>& location, std::vector<std::string>::iterator& it);
	static	void	checkVectorEnd(std::vector<std::string>& vec, std::vector<std::string>::iterator& it, std::string msg);
	static	void	checkSemiColon(std::vector<std::string>& vec, std::vector<std::string>::iterator it, std::string msg);
	static	void	checkDuplicates(std::variant<int, bool, std::string> val, std::string msg);
};
