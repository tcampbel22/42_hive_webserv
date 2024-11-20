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

#include "../include/webserv.hpp"
#include "../include/HttpServer.hpp"
#include "../include/ConfigParser.hpp"
#include "../include/ServerSettings.hpp"


void	ft_perror(std::string str) //need to make as logger instead
{
	std::cerr << "webserv: " << str << std::endl;
}

int	main(int ac, char **av)
{
	if (ac != 2)
	{
		ft_perror("expecting only configuration file as argument");
		return 1;
	}
	//Program will exit if an error is found with the config file
	ConfigParser config((std::string)av[1]);
	std::shared_ptr<ServerSettings> settings = std::make_shared<ServerSettings>(); 
	// try {
		config.parseConfigFile();
		std::cout << config.settings[0].getHost() << '\n';
		std::cout << config.settings[0].getLocationRoot("/docs") << '\n';
		std::cout << config.settings[0].getLocationPath("/static") << '\n';
	// }
	// catch (std::exception& e) {
	// 	std::cerr << e.what() << '\n';
	// 	exit (1);
	// }
	/*start server class, calls the socket creation function in constructor, closes the socket in
	the destructor.
	*/
	HttpServer server(settings);
	server.startListening();

	//exit

	return (0);
}