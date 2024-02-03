#include <Utility/StringTransform.hpp>

#include <cassert>

#include <unicode/unistr.h>

std::string ToLowerCase(std::string_view str) noexcept {
    icu::UnicodeString ustr(str.data());
    assert(!ustr.isBogus());
    std::string rv;
    ustr.toLower().toUTF8String(rv);
    return rv;
}
std::string ToUpperCase(std::string_view str) noexcept {
    icu::UnicodeString ustr(str.data());
    assert(!ustr.isBogus());
    std::string rv;
    ustr.toUpper().toUTF8String(rv);
    return rv;
}