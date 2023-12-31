#include <Utility/StringMap.hpp>

size_t detail::string_hash::operator()(std::string_view txt) const noexcept { return hash_type{}(txt); }
size_t detail::string_hash::operator()(const std::string& txt) const noexcept { return hash_type{}(txt); }
size_t detail::string_hash::operator()(const char* txt) const noexcept { return hash_type{}(txt); }

bool detail::string_equal::operator()(std::string_view l, std::string_view r) const noexcept { return l == r; }
bool detail::string_equal::operator()(std::string_view l, const std::string& r) const noexcept { return l == r; }
bool detail::string_equal::operator()(std::string_view l, const char* r) const noexcept { return l == r; }

bool detail::string_equal::operator()(const std::string& l, std::string_view r) const noexcept { return l == r; }
bool detail::string_equal::operator()(const std::string& l, const std::string& r) const noexcept { return l == r; }
bool detail::string_equal::operator()(const std::string& l, const char* r) const noexcept { return l == r; }

bool detail::string_equal::operator()(const char* l, std::string_view r) const noexcept { return l == r; }
bool detail::string_equal::operator()(const char* l, const std::string& r) const noexcept { return l == r; }
bool detail::string_equal::operator()(const char* l, const char* r) const noexcept { return l == r; }