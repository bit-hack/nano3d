#pragma once

#include <stdint.h>

#if defined(_MSC_VER)
#include <intrin.h>
#endif

// include sse intrinsics
#include <xmmintrin.h>

#if defined(_MSC_VER)
#define n3d_assert(X)       \
    {                       \
        if (!(X))           \
            __debugbreak(); \
    }
#else
#if 1
#include <signal.h>
#include <unistd.h>
#define n3d_assert(X)       \
    {                       \
        if (!(X))           \
            raise(SIGTRAP); \
    }
#else
#define n3d_assert(X)           \
    {                           \
        if (!(X))               \
            __asm__("int3 \n"); \
    }
#endif
#endif

template <typename type_t>
static inline type_t* checked(type_t* x)
{
    n3d_assert(x);
    return x;
}

static inline constexpr
bool power_of_two(uint32_t x) {
    return ((x - 1) & x) == 0;
}

template <typename type_t>
inline constexpr type_t max2(const type_t a, const type_t b)
{
    return (a > b) ? a : b;
}

template <typename type_t>
inline constexpr type_t min2(const type_t a, const type_t b)
{
    return (a < b) ? a : b;
}

template <typename type_t>
inline constexpr type_t max3(const type_t a, const type_t b, const type_t c)
{
    return max2(a, max2(b, c));
}

template <typename type_t>
inline constexpr type_t min3(const type_t a, const type_t b, const type_t c)
{
    return min2(a, min2(b, c));
}

template <typename type_t>
inline constexpr type_t clamp(type_t lo, type_t in, type_t hi)
{
    return (in < lo) ? lo : ((in > hi) ? hi : in);
}
