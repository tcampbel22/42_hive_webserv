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


#include <iostream>
#include <fstream>
#include <ctime>
#include <filesystem>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m" 
#define BLUE    "\033[34m" 

enum e_log
{
	ERROR,
	INFO,
};

class Logger
{
private:
	static std::ofstream log_file;
public:
	Logger();
	~Logger();
	static std::string 		getCurrentTime();
	static void 			log(std::string msg, e_log log_code);
	static bool				checkFileSize();
	static void 			closeLogger();
};