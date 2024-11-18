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

#include "../include/GlobalSettings.hpp"
#include "../include/ConfigParser.hpp"

GlobalSettings::GlobalSettings() 
{
	
	addGlobalSetting("host", "127.0.0.1");
	addGlobalSetting("port", 8000);
	addGlobalSetting("max_client_body_size", 5);
	addErrorPage(404, "/root/var/www/html/404.html");
	addErrorPage(500, "/root/var/www/html/404.html");
	addGlobalSetting("error_pages", error_pages);

	// regexList.push_back(std::regex("[Hh][o][s][t]"));
	// regexList.push_back(std::regex("[Pp][o][r][t]"));
	// regexList.push_back(std::regex("[Ss][e][r][v][e][r][_][Nn][a][m][e]"));
	// regexList.push_back(std::regex("[Mm][a][x][_][Cc][l][i][e][n][t][_][Bb][o][d][y]"));
}

GlobalSettings::~GlobalSettings() {}

void		GlobalSettings::parseGlobalSettings(std::string configFile) 
{
	std::regex host("[Hh][o][s][t]");
	std::smatch match;
	if (std::regex_search(configFile, match, host))
	{
		//do something
	}
}

void		GlobalSettings::addGlobalSetting(std::string key, std::variant<std::string, uint, int, std::vector<std::string>, std::unordered_map<uint, std::vector<std::string>>> value)
{
	global_map[key] = value;
}

void	GlobalSettings::addErrorPage(uint status, std::string path)
{
	error_pages[status].push_back(path);
}


// uint		GlobalSettings::getPort()
// {
// 	auto it = global_map.find("port");
// 	if (it != global_map.end())
// 	{
// 		if (std::holds_alternative<uint>(it->second))
// 			return std::get<uint>(it->second); 
// 	}
// 	else
// 		ft_perror("'port' key not found");
// 	return (0);
// }

// std::string	GlobalSettings::getHost() {}

// std::string GlobalSettings::getServerName() {}

// uint		GlobalSettings::getMaxClientBody() {}

// std::vector<std::string> GlobalSettings::get404Error() {}

// std::vector<std::string> GlobalSettings::get500Error() {}