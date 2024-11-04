/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clundber < clundber@student.hive.fi>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 10:14:15 by clundber          #+#    #+#             */
/*   Updated: 2024/11/04 10:18:21 by clundber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

class HttpServer
{
private:
	

public:
	//constructors & destructors
	HttpServer();
	HttpServer(HttpServer&); //does it need to be in orthodox canonical form?
	~HttpServer();
	
	//operator overloads
	HttpServer& operator=(HttpServer&);

	//methods

};