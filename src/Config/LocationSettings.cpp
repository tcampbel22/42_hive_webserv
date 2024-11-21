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
	path = "\\";
	root = "var\\html";
	default_file = "var\\html\\index.html";
	setMethods("POST");
	setMethods("GET");
	autoindex = false;
}

LocationSettings::LocationSettings(const std::string& new_path)
{
	path = new_path;
	root = "var\\html";
	default_file = "var\\html\\index.html";
	setMethods("POST");
	setMethods("GET");
	autoindex = false;
}

LocationSettings::~LocationSettings() {}

//SETTERS
void	LocationSettings::setPath(std::string new_path) { path = new_path; }
void	LocationSettings::setRoot(std::string new_root) { root = new_root; }
void	LocationSettings::setMethods(std::string new_method) { methods.push_back(new_method); }
void	LocationSettings::setDefaultFile(std::string new_filepath) { default_file = new_filepath; }
void	LocationSettings::setAutoIndex(bool val) { autoindex = val; }

//GETTERS
std::string					LocationSettings::getPath() { return path;}
std::string					LocationSettings::getRoot() { return root; }
std::string					LocationSettings::getDefaultFilePath() { return default_file; }
std::vector<std::string>	LocationSettings::getMethods() { return methods; }
bool						LocationSettings::isAutoIndex() { return autoindex; }