#pragma once

#include <string>

inline bool isUpperCase(char chr) {
	return chr >= 'A' && chr <= 'Z';
}

inline std::string Prettify(const std::string& string) {
	std::string rv;
	rv.reserve(64);
	rv.replace(0, 1, 1, string[0] >= 'a' ? (char)toupper(string[0]) : string[0]); // probably incorrect usage
	auto i = 1;
	auto j = 1;
	char currChar;
	char prevChar;
	char nextChar;
	while ((nextChar = string[i + 1]) != 0) {
		prevChar = rv[j - 1];
		currChar = string[i];
		if (isUpperCase(currChar)) {
			if (!isUpperCase(prevChar) || !isUpperCase(nextChar)) {
				rv.replace(j, 1, " ");
				rv.replace(j + 1, 1, 1, currChar);
				i++;
				j += 2;
				continue;
			}
		}
		rv.replace(j, 1, 1, currChar);
		i++;
		j++;
	}
	if (isUpperCase(string[i]) && !isUpperCase(string[i - 1])) {
		rv.replace(j, 1, " ");
		rv.replace(j + 1, 1, 1, string[i]);
	} else {
		rv.replace(j, 1, 1, string[i]);
	}
	return rv;
}