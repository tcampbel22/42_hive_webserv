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
		throw std::runtime_error("config: curly brackets mismatched");
}

void	ConfigUtilities::trimServerBlock(std::vector<std::string>& serverBlock)
{
	serverBlock.pop_back();
	serverBlock[0].erase();
	serverBlock[1].erase();
	serverBlock.shrink_to_fit();
}

void	ConfigUtilities::shiftLocationBlock(std::vector<std::string>& location, std::vector<std::string>::iterator& it)
{
	if ((std::next(it) != location.end() || std::next(it)->compare("}") == 0) && !it->compare("{"))
		it++;
	else
		throw std::runtime_error("location: invalid syntax error");

}
void	ConfigUtilities::checkVectorEnd(std::vector<std::string>& vec, std::vector<std::string>::iterator& it, std::string msg)
{
	if (std::next(it) == vec.end())
		throw std::runtime_error(msg);
	it++;
}

void	ConfigUtilities::checkSemiColon(std::vector<std::string>& vec, std::vector<std::string>::iterator it, std::string msg)
{
	if (std::next(it) == vec.end() || (it + 1)->compare(";"))
		throw std::runtime_error(msg);
}

void	ConfigUtilities::checkDuplicates(std::variant<int, bool, std::string> val, std::string msg)
{
	if (auto ptr = std::get_if<int>(&val))
	{
		if (*ptr != -1)
			throw std::runtime_error(msg + " duplicate error");
	}
	else if (auto ptr = std::get_if<bool>(&val))
	{
		if (*ptr == true)
			throw std::runtime_error(msg + " duplicate error");
	}
	else if (auto ptr = std::get_if<std::string>(&val))
	{
		if (!ptr->empty())
			throw std::runtime_error(msg + " duplicate error");
	}
}
