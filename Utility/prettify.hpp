#pragma once

#include <string>

inline std::string Prettify(const std::string& string) {
	std::string rv;
	rv.reserve(64);
	rv.replace(0, 1, 1, string[0] >= 'a' ? (char)toupper(string[0]) : string[0]); // probably incorrect usage
	auto i = 1;
	auto j = 1;
	char ch;
	while ((ch = string[i]) != 0) {
		if (ch >= 'A' && ch <= 'Z') {
			rv.replace(j, 1, " ");
			rv.replace(j + 1, 1, 1, ch);
			j++;
		}
		else {
			rv.replace(j, 1, 1, ch);
		}
		i++;
		j++;
	}
	return rv;
}