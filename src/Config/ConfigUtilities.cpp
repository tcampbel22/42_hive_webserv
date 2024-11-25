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

#include "ConfigUtilities.hpp"

ConfigUtilities::ConfigUtilities() {}
ConfigUtilities::~ConfigUtilities() {}

void	ConfigUtilities::checkBrackets(std::vector<std::string> serverBlock) 
{
	int			lb_count = 0;
	int			rb_count = 0;
	for (auto it = serverBlock.begin(); it != serverBlock.end(); it++)
	{
		if (it->compare("{"))
			lb_count++;
		if (it->compare("}"))
			rb_count++;
	}
	if (lb_count != rb_count)
		throw std::runtime_error("Curly brackets mismatched");
}

void	ConfigUtilities::trimServerBlock(std::vector<std::string>& serverBlock)
{
	serverBlock.pop_back();
	serverBlock[0].erase();
	serverBlock[1].erase();
	serverBlock.shrink_to_fit();
}
