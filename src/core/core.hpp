#pragma once

//#include "pbr_concepts.hpp"

#include <cstdint>

using i32 = std::int32_t;
using f32 = float;
using f64 = double;


using fp_t = f32;


#ifndef PBR_DISTRIBUTION
    #define PBR_ENABLE_ASSERTS
#endif

#ifdef PBR_ENABLE_ASSERTS
    #include <cstdlib>
    #define PBR_ASSERT(EXPRESSION)   \
        { if(!(EXPRESSION)) {           \
            abort();  \
        } }
#else
    #define PBR_ASSERT(EXPRESSION)
#endif
