/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: casimirri <clundber@student.hive.fi>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 13:56:56 by clundber          #+#    #+#             */
/*   Updated: 2024/11/15 17:34:41 by casimirri        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include <iostream>
#include "Response.hpp"

// class Response;

class ServerHandler
{
private:
	//InputinformationClass& Eromon;	


	//input information

	// std::string _accept[20];
	
	// std::string _setPath;
	// std::string	_getPAth;
	std::string _type = "POST";
	bool	_error = false;
	Response _response;
	// std::string _body;
	// bool	_connection:
	


public:
	ServerHandler(int fd);//(InputinformationClass& Eromon)
	void executeInput();
	void doPost();
	void doDelete();
	void doGet();
	void doError();
	void getFile(std::string path);
};