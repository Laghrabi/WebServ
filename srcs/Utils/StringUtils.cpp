/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/15 12:47:03 by claghrab          #+#    #+#             */
/*   Updated: 2026/06/08 14:20:57 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "StringUtils.hpp"

std::string	trimSpaces(const std::string& str) {
	size_t start = str.find_first_not_of(" \t");
	
	if (start == std::string::npos)
		return ("");

	size_t	end = str.find_last_not_of(" \t");
	
	return (str.substr(start, end - start + 1));
}