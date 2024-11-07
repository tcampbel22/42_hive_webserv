/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcampbel <tcampbel@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 14:43:31 by tcampbel          #+#    #+#             */
/*   Updated: 2024/11/07 15:15:00 by tcampbel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once 

# include "../include/HttpServer.hpp"
# include <iostream>
# include <sstream>
# include <fstream>

class HttpParser
{
public:
	static void	bigSend(int fd, std::string path);
};