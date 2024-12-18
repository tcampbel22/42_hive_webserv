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

#include "../../include/webserv.hpp"
#include "../Server/HttpServer.hpp"
#include "../Config/ConfigParser.hpp"
#include "../Config/ServerSettings.hpp"
#include "../HttpParsing/HttpParser.hpp"
#include "../Logger/Logger.hpp"


void	ft_perror(std::string str) //need to make as logger instead
{
	std::cerr << "webserv: " << str << std::endl;
}

int	main(int ac, char **av)
{
	Logger log;
	if (ac != 2 || av[1] == nullptr || av[1][0] == '\0')
	{
		ft_perror("expecting only configuration file as argument");
		Logger::log("expecting only configuration file as argument",  ERROR);
		return 1;
	}
	std::string infile(av[1]);
	try {
		if (opendir(av[1]) != NULL)
			throw std::invalid_argument("argument is a directory");
	} 
	catch (std::exception& e)
	{
		Logger::log(e.what(), ERROR);
		ft_perror(e.what());
		return 1; 
	}
	//Program will exit if an error is found with the config file
	ConfigParser config;
	try {
		config.parseConfigFile(infile);
	}
	catch (std::exception& e)
	{
		Logger::log(e.what(), ERROR);
		ft_perror(e.what());
		return 1;
	}
	//start server class, calls the socket creation function in constructor, closes the socket in the destructor.
	HttpServer server(config.settings_vec);
	server.startListening();
	Logger::closeLogger();
	std::cout << "LOOPOUT\n";
	return (0);
}