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
#include "../include/GlobalSettings.hpp"


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
	ConfigParser settings((std::string)av[1]);
	settings.parseConfigFile();
	std::cout << settings.getGlobal<std::string>("host") << std::endl;
	std::cout << settings.getGlobal<int>("port") << std::endl;
	//parsing
	
	/*start server class, calls the socket creation function in constructor, closes the socket in
	the destructor.
	*/
	HttpServer server(settings.getGlobal<std::string>("host"), settings.getGlobal<int>("port"));
	server.startListening();

	//exit

	return (0);
}