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

# include "../include/HttpParser.hpp"


void	HttpParser::bigSend(int out_fd, std::string path) 
{
	std::string			buf;
	std::ostringstream	stream;
	std::fstream 		infile;

	infile.open(path);
	if (!infile.is_open())
	{
		std::cerr << "File failed to open\n";
		return ;
	}
	stream << infile.rdbuf();
	buf = stream.str();
	std::cout << buf << " Buffer\n";
	infile.close();
	if (buf.find("GET"))
		send(out_fd, buf.c_str(), buf.size(), 0);
}