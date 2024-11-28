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

#include "LocationSettings.hpp"

LocationSettings::LocationSettings()
{
	path = "/root";
	root = "";
	default_file = "";
	autoindex = false;
	redirect = false;
}


LocationSettings::LocationSettings(const std::string& new_path)
{
	path = new_path;
	if (path.back() == '/')
		isDirectory = true;
	else
		isFile = true;
	root = "";
	default_file = "";
	autoindex = false;
	redirect = "";
}

LocationSettings::~LocationSettings() {}

//PARSERS

void	LocationSettings::checkLocationValues(std::vector<std::string>::iterator& it)
{
	if (!std::next(it)->compare("}"))
	{
		if (!redirect.empty() && (!root.empty() || autoindex || !methods.empty() || !default_file.empty()))
			throw std::runtime_error("location: extra directives in redirect block");
		if (!path.compare("/"))
		{
			if (default_file.empty())
				throw std::runtime_error("location: default file missing from root directory");
		}
		if (!path.compare("/temp/"))
		{
			if (methods.empty())
				throw std::runtime_error("location: methods missing from /temp/ directory");
		}
		if (root.empty())
			throw std::runtime_error("location: root path missing");
	}
}

void	LocationSettings::parseRoot(std::vector<std::string>& location, std::vector<std::string>::iterator& it) 
{
	ConfigUtilities::checkDuplicates(root, "root:");
	ConfigUtilities::checkVectorEnd(location, it, "location: root: invalid syntax");
	std::regex root_regex("\\/[a-zA-Z0-9._\\-\\/]+");
	if (std::regex_match(*it, root_regex) && std::next(it)->compare(";") == 0)
		root = *it;
	else
		throw std::runtime_error("location: root: invalid root path/syntax error");
	ConfigUtilities::checkVectorEnd(location, it, "location: root: invalid syntax");
	checkLocationValues(it);
}

void	LocationSettings::parseDefaultFile(std::vector<std::string>& location, std::vector<std::string>::iterator& it) 
{ 
	ConfigUtilities::checkDuplicates(default_file, "default file:");
	ConfigUtilities::checkVectorEnd(location, it, "location: default file: invalid syntax");
	ConfigUtilities::checkSemiColon(location, it, "location: default file: syntax error");
	default_file = *it;
	checkLocationValues(it);
	ConfigUtilities::checkVectorEnd(location, it, "location: autoindex: invalid syntax");
}

void	LocationSettings::parseAutoIndex(std::vector<std::string>& location, std::vector<std::string>::iterator& it) 
{
	ConfigUtilities::checkDuplicates(autoindex, "autoindex:");
	ConfigUtilities::checkVectorEnd(location, it, "location: autoindex: invalid syntax");
	ConfigUtilities::checkSemiColon(location, it, "location: autoindex: syntax error");
	if (!it->compare("on"))
		autoindex = true;
	else if (!it->compare("off"))
		autoindex = false;
	else
		throw std::runtime_error("location: autoindex: value must be on or off");
	checkLocationValues(it);
	ConfigUtilities::checkVectorEnd(location, it, "location: autoindex: invalid syntax");
}

void	LocationSettings::parseRedirect(std::vector<std::string>& location, std::vector<std::string>::iterator& it)
{
	ConfigUtilities::checkDuplicates(redirect, "redirect:");
	ConfigUtilities::checkVectorEnd(location, it, "location: redirect: invalid syntax");
	ConfigUtilities::checkSemiColon(location, it, "location: redirect: syntax error");
	redirect = *it;
	checkLocationValues(it);
	ConfigUtilities::checkVectorEnd(location, it, "location: autoindex: invalid syntax");
}

void	LocationSettings::parseMethods(std::vector<std::string>& location, std::vector<std::string>::iterator& it)
{
	ConfigUtilities::checkDuplicates(methods, "methods:");
	ConfigUtilities::checkVectorEnd(location, it, "location: methods: syntax error");
	std::string method_str[3] = {"GET", "POST", "DELETE"};
	for (; it != location.end(); it++)
	{
		if (!it->compare(";"))
			break ;
		if (!it->compare(method_str[0]))
			methods.push_back(GET);
		else if (!it->compare(method_str[1]))
			methods.push_back(POST);
		else if (!it->compare(method_str[2]))
			methods.push_back(DELETE);
		else 
			throw std::runtime_error("location: methods: invaid method type");
	}
	ConfigUtilities::checkSemiColon(location, std::prev(it), "location: redirect: syntax error");
	ConfigUtilities::checkMethodDuplicates(methods);
	checkLocationValues(it);
}

//GETTERS
std::string&				LocationSettings::getPath() { return path; }
std::string&				LocationSettings::getRoot() { return root; }
std::string&				LocationSettings::getDefaultFile() { return default_file; }
std::string&				LocationSettings::getRedirect() { return redirect; }
std::vector<int>&			LocationSettings::getMethods() { return methods; }
bool						LocationSettings::isAutoIndex() { return autoindex; }