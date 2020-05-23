#pragma once

#include <cmath>
#include <algorithm>
#include <limits>
#include <numbers>

#include <core/core.hpp>


namespace pbr {

constexpr fp_t c_Infinity = std::numeric_limits<fp_t>::infinity();


template <typename T> inline
bool isNaN(const T x)
{
    if constexpr (std::numeric_limits<T>::is_iec559)
        return std::isnan(x);
    else
        return false;
}


template<std::floating_point T> inline
T sqrt(const T v)
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
//
//template<typename T>
//inline T sin(const T v)
//{
//    static_assert(std::numeric_limits<T>::is_iec559);
//
//    if constexpr (std::is_same<T, float>())
//        return std::sinf(v);
//    else
//        return std::sin(v);
//}
//
//template<typename T>
//inline T cos(const T v)
//{
//    static_assert(std::numeric_limits<T>::is_iec559);
//
//    if constexpr (std::is_same<T, float>())
//        return std::cosf(v);
//    else
//        return std::cos(v);
//}
//
//template<typename T>
//inline T tan(const T v)
//{
//    static_assert(std::numeric_limits<T>::is_iec559);
//
//    if constexpr (std::is_same<T, float>())
//        return std::tanf(v);
//    else
//        return std::tan(v);
//}
//
//
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
//
//
//template<typename T>
//inline constexpr T radians(T degrees)
//{
//    static_assert(std::numeric_limits<T>::is_iec559);
//
//    return degrees * std::numbers::pi_v<T> / 180;
//}


template<std::floating_point T> inline
T floor(const T v)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    if constexpr (std::is_same<T, f32>())
        return std::floorf(v);
    else if constexpr (std::is_same<T, f64>())
        return std::floor(v);
}

template<std::floating_point T> inline
T ceil(const T v)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    if constexpr (std::is_same<T, f32>())
        return std::ceilf(v);
    else if constexpr (std::is_same<T, f64>())
        return std::ceil(v);
}


template<typename T> constexpr inline
T max3(const T x, const T y, const T z)
{
    return std::max(x, std::max(y, z));
}

template<typename T> constexpr inline
T min3(const T x, const T y, const T z)
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


} // namespace pbr
