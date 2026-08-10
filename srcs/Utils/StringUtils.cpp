/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/15 12:47:03 by claghrab          #+#    #+#             */
/*   Updated: 2026/07/08 20:16:45 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/webserver.hpp"

std::string	trimSpaces(const std::string& str) {
	if (!str.empty()) {
		size_t start = str.find_first_not_of(" \t");
		if (start == std::string::npos)
			return ("");
	
		size_t	end = str.find_last_not_of(" \t");
		std::string trimed = str.substr(start, end - start + 1);
		return (trimed);
	} else {
		return ("");
	}
}