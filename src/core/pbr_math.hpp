#pragma once

#include <cmath>
#include <algorithm>
#include <numbers>
#include <limits>
#include "core.hpp"

// TODO: Use C++ 20 concepts
// NOTE: Apparently all this wrappers are useless, because c++ variants of this functions are overloaded.
//       But may be this will be usefull with benchmarking f32 vs f64, where I can use f64 versions of this functions with fp_t=f32.
// NOTE: Found out about std::hypot(), which can compute sqrt(x^2+y^2),sqrt(x^2+y^2+z^2) with better precision, but like 20x slower.
//       Another interesting function is std::fma(), although smae assembly can be achieved with multiple optimization flags.
//       https://stackoverflow.com/questions/34265982/automatically-generate-fma-instructions-in-msvc
// TODO: '__forceinline' instead of 'inline' ? Although it is not a big difference.

PBR_NAMESPACE_BEGIN

template<typename T> inline
T Sqrt(const T v)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    if constexpr (std::is_same<T, f32>())
        return std::sqrtf(v);
    else if constexpr (std::is_same<T, f64>())
        return std::sqrt(v);
}

//template<typename T>
//inline T cbrt(const T v)
//{
//    static_assert(std::numeric_limits<T>::is_iec559);
//
//    if constexpr (std::is_same<T, float>())
//        return std::cbrtf(v);
//    else
//        return std::cbrt(v);
//}

template<typename T> inline
T Sin(const T v)
{
   static_assert(std::numeric_limits<T>::is_iec559);

   if constexpr (std::is_same<T, f32>())
       return std::sinf(v);
   else
       return std::sin(v);
}

template<typename T> inline
T Cos(const T v)
{
   static_assert(std::numeric_limits<T>::is_iec559);

   if constexpr (std::is_same<T, f32>())
       return std::cosf(v);
   else
       return std::cos(v);
}

template<typename T> inline
T ACos(const T v)
{
   static_assert(std::numeric_limits<T>::is_iec559);

   if constexpr (std::is_same<T, f32>())
       return std::acosf(v);
   else
       return std::acos(v);
}

//template<typename T> inline
//inline T tan(const T v)
//{
//    static_assert(std::numeric_limits<T>::is_iec559);
//
//    if constexpr (std::is_same<T, float>())
//        return std::tanf(v);
//    else
//        return std::tan(v);
//}

template<typename T> inline
T ATan2(const T y, const T x)
{
   static_assert(std::numeric_limits<T>::is_iec559);

   if constexpr (std::is_same<T, f32>())
       return std::atan2f(y, x);
   else
       return std::atan2(y, x);
}


template<typename T> inline
T Floor(const T v)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    if constexpr (std::is_same<T, f32>())
        return std::floorf(v);
    else if constexpr (std::is_same<T, f64>())
        return std::floor(v);
}

template<typename T> inline
T Ceil(const T v)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    if constexpr (std::is_same<T, f32>())
        return std::ceilf(v);
    else if constexpr (std::is_same<T, f64>())
        return std::ceil(v);
}


template<typename T> inline constexpr
T Max3(const T x, const T y, const T z)
{
    T result = z;
    if (y > result) result = y;
    if (x > result) result = x;
    return result;
}

template<typename T> inline constexpr
T Min3(const T x, const T y, const T z)
{
    // NOTE: For some fucking reason msvc can't optimize this to two instructions.
    //return std::min(z, std::min(y, x));
    // This, however, will be optimized on all compilers. That is only for f32 and f64.
    // FINDOUT: If there will be any noticable difference at all.
    // FINDOUT: Turns out that if I compile with 'gcc -O3 -march=native', it should be T result = x etc. So it's not that simple.
    T result = z;
    if (y < result) result = y;
    if (x < result) result = x;
    return result;
}


// PBR book functions
template<typename T> inline constexpr
T Radians(const T degrees)
{
   static_assert(std::numeric_limits<T>::is_iec559);
   return degrees * std::numbers::pi_v<T> / static_cast<T>(180);
}

template<typename Float> inline constexpr
Float Lerp(const Float t, const Float f1, const Float f2)
{
    static_assert(std::numeric_limits<Float>::is_iec559);
    return f1 - t * (f2 - f1);
}

inline constexpr
fp_t Gamma(i32 n)
{
    return (n * constants::machineEpsilon) / ( 1 - n * constants::machineEpsilon);
}


PBR_NAMESPACE_END
