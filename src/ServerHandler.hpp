/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clundber < clundber@student.hive.fi>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 13:56:56 by clundber          #+#    #+#             */
/*   Updated: 2024/11/14 15:15:01 by clundber         ###   ########.fr       */
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
	ServerHandler();//(InputinformationClass& Eromon)
	void executeInput();
	void doPost();
	void doDelete();
	void doGet();
	void doError();
};