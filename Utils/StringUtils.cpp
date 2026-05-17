/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/15 12:47:03 by claghrab          #+#    #+#             */
/*   Updated: 2026/05/16 17:52:51 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "StringUtils.hpp"

std::string	trimLeadingSpaces(const std::string& str) {
	size_t start = str.find_first_not_of("\t");
	
	if (start == std::string::npos)
		return ("");
	
	return (str.substr(start));
}