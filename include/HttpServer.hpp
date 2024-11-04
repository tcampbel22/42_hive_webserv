/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clundber < clundber@student.hive.fi>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 10:14:15 by clundber          #+#    #+#             */
/*   Updated: 2024/11/04 10:34:56 by clundber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include <string>

class HttpServer
{
private:
	std::string _ipAdress;
	int		_port;
	int		_socket;
	
public:
	//constructors & destructors
	HttpServer();
	HttpServer(/*socket information*/);
	HttpServer(HttpServer&); //does it need to be in orthodox canonical form?
	~HttpServer();
	//operator overloads
	HttpServer& operator=(HttpServer&);

	//methods

};