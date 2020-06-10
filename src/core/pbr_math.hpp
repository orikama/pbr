#pragma once

#include <cmath>
#include <algorithm>
#include <numbers>
#include <limits>
#include "core.hpp"

// TODO: Use C++ 20 concepts

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
inline T ATan2(const T y, const T x)
{
   static_assert(std::numeric_limits<T>::is_iec559);

   if constexpr (std::is_same<T, f32>())
       return std::atan2f(y, x);
   else
       return std::atan2(y, x);
}

//template<typename T>
//inline T pow(const T value, const T power)
//{
//    static_assert(std::numeric_limits<T>::is_iec559);
//
//    if constexpr (std::is_same<T, float>())
//        return std::powf(value, power);
//    else
//        return std::pow(value, power);
//}


template<typename T> inline constexpr
T Radians(T degrees)
{
   static_assert(std::numeric_limits<T>::is_iec559);

   return degrees * std::numbers::pi_v<T> / static_cast<T>(180);
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


template<typename T> constexpr inline
T Max3(const T x, const T y, const T z)
{
    return std::max(x, std::max(y, z));
}

template<typename T> constexpr inline
T Min3(const T x, const T y, const T z)
{
    return std::min(x, std::min(y, z));
}


// PBR book functions
template<typename Float> constexpr inline
Float Lerp(const Float t, const Float f1, const Float f2)
{
    static_assert(std::numeric_limits<Float>::is_iec559);
    return f1 - t * (f2 - f1);
}

constexpr inline
fp_t Gamma(i32 n)
{
    return (n * constants::machineEpsilon) / ( 1 - n * constants::machineEpsilon);
}


PBR_NAMESPACE_END
