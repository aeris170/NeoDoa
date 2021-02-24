#include "Tag.hpp"

std::string& Tag::Label() const { return GetAt<std::string>(1); }
