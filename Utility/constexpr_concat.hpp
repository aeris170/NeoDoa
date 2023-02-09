#pragma once

#include <initializer_list>

// we cannot return a char array from a function, therefore we need a wrapper
template <unsigned N>
struct String {
    char c[N];

    inline operator const char*() { return c; }
};

template<unsigned ...Len>
constexpr auto cat(const char(&...strings)[Len]) {
    constexpr unsigned N = (... + Len) - sizeof...(Len);
    String<N + 1> result = {};
    result.c[N] = '\0';

    char* dst = result.c;
    for (const char* src : { strings... }) {
        for (; *src != '\0'; src++, dst++) {
            *dst = *src;
        }
    }
    return result;
}