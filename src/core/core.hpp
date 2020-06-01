#pragma once

//#include "pbr_concepts.hpp"

#include <limits>
#include <cstdint>

using i32 = std::int32_t;
using i64 = std::int64_t;
using f32 = float;
using f64 = double;


using fp_t = f32;


#ifndef PBR_DISTRIBUTION
    #define PBR_ENABLE_ASSERTS
#endif

#ifdef PBR_ENABLE_ASSERTS
    #include <cstdlib>
    #include <cstdio>
    #define PBR_ASSERT(EXPRESSION) \
        { if(!(EXPRESSION)) { \
            printf("Assertion Failed: %s\n\tFile: %s\n\tLine: %i", #EXPRESSION, __FILE__, __LINE__); \
            abort(); \
        } }
    #define PBR_ASSERT_MSG(EXPRESSION, MESSAGE) \
        { if(!(EXPRESSION)) { \
            printf("Assertion Failed: %s\n\tFile: %s\n\tLine: %i\n\tMessage: %s", #EXPRESSION, __FILE__, __LINE__, MESSAGE); \
            abort(); \
        } }
#else
    #define PBR_ASSERT(EXPRESSION)
#endif


namespace pbr::constants {
    inline constexpr fp_t infinity = std::numeric_limits<fp_t>::infinity();
} // namespace pbr::constants
