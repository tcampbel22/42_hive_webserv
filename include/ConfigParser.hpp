/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcampbel <tcampbel@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 13:35:03 by tcampbel          #+#    #+#             */
/*   Updated: 2024/11/13 14:35:45 by tcampbel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include <unordered_map>
#include <iostream>

class ConfigParser
{
private:
	std::unordered_map<std::string, std::string> hash_table;
public:
	ConfigParser();
	~ConfigParser();
};