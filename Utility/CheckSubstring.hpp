#pragma once

#include <utility>
#include <string_view>

bool CheckSubstring          (const std::string_view text, const std::string_view query);
bool CheckSubstringIgnoreCase(const std::string_view text, const std::string_view query);

int FindSubstringIndex          (const std::string_view text, const std::string_view query);
int FindSubstringIndexIgnoreCase(const std::string_view text, const std::string_view query);