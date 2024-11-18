/************************************************/
/** __          __  _                          **/
/** \ \        / / | |                         **/
/**  \ \  /\  / /__| |__  ___  ___ _ ____   __ **/
/**   \ \/  \/ / _ \ '_ \/ __|/ _ \ '__\ \ / / **/
/**    \  /\  /  __/ |_) \__ \  __/ |   \ V /  **/
/**     \/  \/ \___|_.__/|___/\___|_|    \_/   **/
/**                                            **/
/**                                            **/
/**             W E B S E R V                  **/
/************************************************/

#include "../include/webserv.hpp"
#include "../include/HttpServer.hpp"


void	ft_perror(std::string str)
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
	std::string confFile = av[1];
	//parsing
	
	/*start server class, calls the socket creation function in constructor, closes the socket in
	the destructor.
	*/
	// HttpServer server("127.0.0.1", 8000);
	// server.startListening();
	int fd = open("./test.txt", O_RDONLY);
	HttpParser parser;
	parser.bigSend(fd);
	

	//exit

	return (0);
}