#pragma once

#include "cstdint"
#include "xhash"

namespace glass::hash {
    template <class T>
    void hashCombine(uint64_t& seed, const T& v) {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    // FNV-1a constexpr hashing functions
    constexpr uint64_t hash32(const char* str, uint64_t n, uint32_t basis = UINT32_C(2166136261)) {
        if (n == 0)
            return basis;

        return hash32(str + 1, n - 1, (basis ^ str[0]) * UINT32_C(16777619));
    }

    constexpr uint64_t hash32(const wchar_t* str, uint64_t n, uint32_t basis = UINT32_C(2166136261)) {
        if (n == 0)
            return basis;

        return hash32(str + 1, n - 1, (basis ^ str[0]) * UINT32_C(16777619));
    }

    constexpr uint64_t hash64(const char* str, uint64_t n, uint64_t basis = UINT64_C(14695981039346656037)) {
        if (n == 0)
            return basis;

        return hash64(str + 1, n - 1, (basis ^ str[0]) * UINT64_C(1099511628211));
    }

    constexpr uint64_t hash64(const wchar_t* str, uint64_t n, uint64_t basis = UINT64_C(14695981039346656037)) {
        if (n == 0)
            return basis;

        return hash64(str + 1, n - 1, (basis ^ str[0]) * UINT64_C(1099511628211));
    }

    template <uint64_t N>
    constexpr uint64_t hash32(const char (&s)[N]) {
        return hash32(s, N - 1);
    }
    template <uint64_t N>
    constexpr uint64_t hash32(const wchar_t (&s)[N]) {
        return hash32(s, N - 1);
    }

    template <uint64_t N>
    constexpr uint64_t hash64(const char (&s)[N]) {
        return hash64(s, N - 1);
    }
    template <uint64_t N>
    constexpr uint64_t hash64(const wchar_t (&s)[N]) {
        return hash64(s, N - 1);
    }

    struct CTStringHash {
        uint64_t value;

        template <uint64_t N>
        constexpr CTStringHash(const char (&str)[N])
            : value(hash64<N>(str)) {}
        template <uint64_t N>
        constexpr CTStringHash(const wchar_t (&str)[N])
            : value(hash64<N>(str)) {}

        constexpr operator uint64_t() const { return value; }
    };
}