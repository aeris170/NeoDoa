#pragma once

#include <string>

// trim from left
std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v");
// trim from right
std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v");
// trim from left & right
std::string& trim(std::string& s, const char* t = " \t\n\r\f\v");

// copying versions
std::string ltrim_copy(std::string s, const char* t = " \t\n\r\f\v");
std::string rtrim_copy(std::string s, const char* t = " \t\n\r\f\v");
std::string trim_copy(std::string s, const char* t = " \t\n\r\f\v");