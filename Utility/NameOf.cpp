#include <Utility/NameOf.hpp>

#include <regex>
#include <stdexcept>

std::string _nameof(const std::string& x) {
    static std::regex regex("^&?([_a-zA-Z]\\w*(->|\\.|::))*([_a-zA-Z]\\w*)$");
    std::smatch match;
    if (std::regex_match(x, match, regex)) {
        if (match.size() == 4) {
            return match[3];
        }
    }
    throw std::logic_error("A bad expression x in nameof(x). The expression is \"" + x + "\".");
}