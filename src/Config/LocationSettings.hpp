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

#include "ServerSettings.hpp"
#include <unordered_map>
#include <iostream>
#include <vector>

class LocationSettings
{
private:
	std::string					path; //URI
	std::string 				root; //directory
	std::vector<int>			methods; //allowed methods
	std::string					default_file; //eg index.html
	bool						autoindex; //enable or disable directory listing
	bool						redirect;
public:
	LocationSettings();
	LocationSettings(const std::string& new_path);
	~LocationSettings();
	void	setPath(std::string new_path);
	void	setRoot(std::string new_root);
	void	setMethods(int new_method);
	void	setDefaultFile(std::string new_filepath);
	void	setAutoIndex(bool val);
	std::string					getPath();
	std::string					getRoot();
	std::string					getDefaultFilePath();
	std::vector<int>			getMethods();
	bool						isAutoIndex();
};