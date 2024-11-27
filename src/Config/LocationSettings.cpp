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
	path = "/";
	root = "root/var/html";
	default_file = "index.html";
	// setMethods(POST);
	// setMethods(GET);
	autoindex = false;
	redirect = false;
}

LocationSettings::LocationSettings(int i)
{
	if (i == 1)
	{
		path = "/path";
		root = "/var/html";
		default_file = "index.html";
		// setMethods(POST);
		// setMethods(GET);
		autoindex = false;
		redirect = false;
	}
	else if (i == 2)
	{
		path = "/path";
		root = "/var/html";
		default_file = "index.html";
		// setMethods(POST);
		// setMethods(GET);
		autoindex = false;
		redirect = false;
	}
	else
	{
		path = "/path";
		root = "/var/html";
		default_file = "index.html";
		// setMethods(POST);
		// setMethods(GET);
		autoindex = false;
		redirect = false;
	}
}

LocationSettings::LocationSettings(const std::string& new_path)
{
	path = new_path;
	if (path.back() == '/')
		isDirectory = true;
	else
		isFile = true;
	// root = "/var/html";
	// default_file = "index.html";
	// setMethods(POST);
	// setMethods(GET);
	// autoindex = false;
	// redirect = false;
}

LocationSettings::~LocationSettings() {}

//PARSERS

void	LocationSettings::checkLocationValues(std::vector<std::string>& location, std::vector<std::string>::iterator& it)
{
	if (std::next(it) == location.end())
	{
		if (redirect && (!root.empty() || autoindex || !methods.empty() || !default_file.empty()))
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
	std::regex root("\\/[a-zA-Z0-9._\\-\\/]+");
	if (std::regex_match(*it, root) && std::next(it)->compare(";") == 0)
		root = *it;
	else
		throw std::runtime_error("location: root: invalid root path/syntax error");
	ConfigUtilities::checkVectorEnd(location, it, "location: root: invalid syntax");
	checkLocationValues(location, it);
}

void	LocationSettings::parseDefaultFile(std::vector<std::string>& location, std::vector<std::string>::iterator& it) 
{ 
	ConfigUtilities::checkDuplicates(default_file, "default file:");
	ConfigUtilities::checkVectorEnd(location, it, "location: default file: invalid syntax");
	ConfigUtilities::checkSemiColon(location, it, "location: default file: syntax error");
	default_file = *it;
	checkLocationValues(location, it);
}

// void	LocationSettings::setRoot(std::string new_root) { root = new_root; }
// void	LocationSettings::setMethods(int new_method) { methods.push_back(new_method); }
// void	LocationSettings::setAutoIndex(bool val) { autoindex = val; }

//GETTERS
std::string&				LocationSettings::getPath() { return path; }
std::string&				LocationSettings::getRoot() { return root; }
std::string&				LocationSettings::getDefaultFile() { return default_file; }
std::vector<int>&			LocationSettings::getMethods() { return methods; }
bool						LocationSettings::isAutoIndex() { return autoindex; }