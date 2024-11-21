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

#include <unordered_map>
#include <iostream>
#include <vector>

class LocationSettings
{
private:
	std::string					path; //URI
	std::string 				root; //directory
	std::vector<std::string>	methods; //allowed methods
	std::string					default_file; //eg index.html
	bool						autoindex; //enable or disable directory listing
public:
	LocationSettings();
	LocationSettings(const std::string& new_path);
	~LocationSettings();
	void	setPath(std::string new_path);
	void	setRoot(std::string new_root);
	void	setMethods(std::string new_method);
	void	setDefaultFile(std::string new_filepath);
	void	setAutoIndex(bool val);
	std::string					getPath();
	std::string					getRoot();
	std::string					getDefaultFilePath();
	std::vector<std::string>	getMethods();
	bool						isAutoIndex();
};