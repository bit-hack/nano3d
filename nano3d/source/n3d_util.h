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

inline constexpr float max2(const float a, const float b)
{
    return (a > b) ? a : b;
}

inline constexpr float min2(const float a, const float b)
{
    return (a < b) ? a : b;
}

inline constexpr float max3(const float a, const float b, const float c)
{
    return max2(a, max2(b, c));
}

inline constexpr float min3(const float a, const float b, const float c)
{
    return min2(a, min2(b, c));
}
