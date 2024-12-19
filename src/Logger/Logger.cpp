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
std::ofstream Logger::log_file;

bool	Logger::checkFileSize()
{
	log_file.seekp(0, std::ios::end);
	int bytes = log_file.tellp();
	if (bytes > 100000)
		return true;
	return false;
}

Logger::Logger()
{
	std::string path = "log";
	std::filesystem::create_directories(path);

	try {
		log_file.open(path + "/log.log", std::ios::app);
		if (!log_file)
			throw std::runtime_error("log file failed to open");
		log_file << "[INFO] " << getCurrentTime() << ": WEBSERV STARTED\n";
		if (checkFileSize())
			throw std::runtime_error("Log file limit exceeded");
	}
	catch (std::exception& e)
	{
		log_file.close();
		std::cerr << e.what() << '\n';
	} 

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
		{
			log_file << "[ERROR] " << getCurrentTime() << ": " << msg << '\n';
			if (checkFileSize())
				throw std::runtime_error("Log file limit exceeded");
		}
		if (log_code == INFO)
		{
			log_file << "[INFO] " << getCurrentTime() << ": " << msg << '\n';
			if (checkFileSize())
				throw std::runtime_error("Log file limit exceeded");
		}
	}
}

void Logger::closeLogger() 
{
    if (log_file.is_open())
	{
		log_file << "[INFO] " << getCurrentTime() << ": WEBSERV FINISHED\n";
		log_file.close();
	}
}

Logger::~Logger() {}