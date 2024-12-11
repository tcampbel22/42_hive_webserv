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

#include "Logger.hpp"

Logger::Logger(const std::string fileName)
{
	std::string path = "log";
	std::filesystem::create_directories(path);

	log_file.open(path + "/" + fileName, std::ios::app);
	if (!log_file)
	{
		// ft_perror("Log file failed to open");
		exit(1);
	}
	log_file << "[INFO] " << getCurrentTime() << ": WEBSERV STARTED\n";
}

std::string Logger::getCurrentTime() 
{
	std::time_t now = std::time(nullptr);
	char buf[100];
	std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
	return std::string(buf);
}

void	Logger::log(std::string msg, e_log log_code)
{
	if (log_file.is_open())
	{
		if (log_code == ERROR)
			log_file<< "[ERROR] " << getCurrentTime() << ": " << msg << '\n';
		if (log_code == INFO)
			log_file << "[INFO] " << getCurrentTime() << ": " << msg << '\n';
	}
}

Logger::~Logger() 
{
	if (log_file.is_open())
	{
		log_file << "[INFO] " << getCurrentTime() << ": WEBSERV FINISHED\n";
		log_file.close();
	}
}