/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestHelpers.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 13:19:17 by claghrab          #+#    #+#             */
/*   Updated: 2026/06/06 13:33:31 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

/**
 * @brief Safely converts a given character to lowercase.
 * * This utility function wraps `std::tolower` to ensure safe type conversion. 
 * By explicitly casting the input to `unsigned char` before passing it to `std::tolower`, 
 * it prevents the undefined behavior that occurs when passing negative signed `char` 
 * values (such as extended ASCII characters) to standard Cctype library functions.
 * * @param c The character to convert.
 * @return The lowercase equivalent of the character if one exists; otherwise, 
 * the character is returned unchanged.
 */
char	safeToLower(char c) {
	return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
}
