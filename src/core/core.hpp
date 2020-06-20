#pragma once

// NOTE: May be should be called PBR_ENABLE_ERROR_CORRECTION
#define PBR_ENABLE_EFLOAT 1
#define PBR_EFLOAT_DEBUG 1

#define PBR_ENABLE_STATS_COUNT 1
#define PBR_ENABLE_PROFILING 1


//#include "pbr_concepts.hpp"

#include <limits>
#include <numbers>
#include <cstdint>

using i32 = std::int32_t;
using ui32 = std::uint32_t;
using i64 = std::int64_t;
using ui64 = std::uint64_t;
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
    #define PBR_ASSERT_MSG(EXPRESSION, MESSAGE)
#endif


#define PBR_NAMESPACE_BEGIN namespace pbr {
#define PBR_NAMESPACE_END }

namespace pbr::constants {
    inline constexpr fp_t infinity = std::numeric_limits<fp_t>::infinity();
    inline constexpr fp_t pi_t = std::numbers::pi_v<fp_t>;
    inline constexpr fp_t machineEpsilon = fp_t(0.5) * std::numeric_limits<fp_t>::epsilon();
} // namespace pbr::constants
