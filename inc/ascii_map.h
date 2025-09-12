#ifndef ASCII_MAP_H
#define ASCII_MAP_H

#include <string>
#include <vector>

/**
 * Converts a string to a vector of strings. Each string is a row in a multiline
 * ASCII text art representation of the string. Each row is also followed by a
 * trailing string of whitespaces
 *
 * @param text              The string to convert
 * @param trailingWidth     The width of the trailing whitespace string to
 *                          append to each row
 * 
 * @returns The vector of row strings for the ASCII text art.
 */
std::vector<std::string> convertToASCIIArt(const std::string &text,
                                           int trailingWidth);

#endif