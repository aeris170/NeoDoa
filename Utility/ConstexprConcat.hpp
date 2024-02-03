#pragma once

// https://stackoverflow.com/a/65440575/9451867
// we cannot return a char array from a function, therefore we need a wrapper
template <unsigned N>
struct String {
    char c[N];

    inline operator const char*() const { return c; }
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