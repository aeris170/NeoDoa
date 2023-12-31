#include <Utility/Split.hpp>

std::vector<std::string> split(std::string str, std::string_view delimeter) {
    std::vector<std::string> rv;
    auto pos = str.find(delimeter);
    while (pos != str.npos) {
        rv.emplace_back(str.substr(0, pos));
        str = str.substr(pos + delimeter.size());
        pos = str.find(delimeter);
    }
    rv.emplace_back(str);
    return rv;
}