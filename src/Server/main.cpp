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


void	ft_perror(std::string str) //need to make as logger instead
{
	std::cerr << "webserv: " << str << std::endl;
}

int	main(int ac, char **av)
{
	if (ac != 2)
	{
		ft_perror("expecting only configuration file as argument"); //check if directory and ./
		return 1;
	}
	try {
		if (opendir(av[1]) != NULL)
			throw std::invalid_argument("argument is a directory");
	} catch (std::exception& e) {
		ft_perror(e.what());
		return 1; }

	//Program will exit if an error is found with the config file
	ConfigParser config((std::string)av[1]);
	try {
		config.parseConfigFile();
	}
	catch (std::exception& e)
	{
		ft_perror(e.what());
		exit(1);
	}
	std::shared_ptr<ServerSettings> setting = std::make_shared<ServerSettings>(config.settings[0]);
	//start server class, calls the socket creation function in constructor, closes the socket in the destructor.
	HttpServer server(setting);
	// server.startListening();
	// int fd = open("./test.txt", O_RDONLY);
	// HttpParser parser;
	// parser.bigSend(fd);
	return (0);
}