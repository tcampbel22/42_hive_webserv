/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clundber < clundber@student.hive.fi>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 13:57:42 by clundber          #+#    #+#             */
/*   Updated: 2024/11/14 15:22:53 by clundber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerHandler.hpp"

ServerHandler::ServerHandler(): //(InputinformationClass& Eromon)
_response()
{
	//InputClass = Eromon; 
	 
	executeInput();

}

void ServerHandler::executeInput()
{
	
	if (_error == true)
		doError();
	else if (_type == "POST")
		doPost();
	else if (_type == "GET")
		doGet();
	else if (_type == "DELETE")
		doDelete();
	else
		throw std::invalid_argument("Invalid argument");
		




}

void 

void ServerHandler::doError()
{
	_response.setResponseCode(400);

	//should get a error file from the directory
	_response.setContentType("text/html");
	_response.setContentLength()
}

void ServerHandler::doPost()
{
	
}

void ServerHandler::doGet()
{
	
}

void ServerHandler::doDelete()
{
	
}