#pragma once

#include <compare>

#include "core.hpp"
#include "pbr_math.hpp"


#define PBR_CNSTEXPR constexpr
#define PBR_INLINE inline


template <typename T> inline static
bool isNaN(const T x)
{
    if constexpr (std::numeric_limits<T>::is_iec559)
        return std::isnan(x);
    else
        return false;
}


PBR_NAMESPACE_BEGIN

// ******************************************************************************
// ---------------------------------- VECTOR2 -----------------------------------
// ******************************************************************************

#pragma region Vector2

template<typename T>
struct Vector2
{
    using Vector2_arg = Vector2<T>&;

    PBR_CNSTEXPR Vector2();
    PBR_CNSTEXPR explicit Vector2(T value);
    PBR_CNSTEXPR explicit Vector2(T x, T y);


    PBR_CNSTEXPR PBR_INLINE Vector2<T>& operator+=(const Vector2_arg v);
    PBR_CNSTEXPR PBR_INLINE Vector2<T>& operator-=(const Vector2_arg v);

    PBR_CNSTEXPR PBR_INLINE Vector2<T>& operator*=(const T scalar);
    PBR_CNSTEXPR PBR_INLINE Vector2<T>& operator/=(const T scalar);

    friend auto operator<=>(const Vector2<T>, const Vector2<T>) = default;


    PBR_CNSTEXPR PBR_INLINE T LengthSquared() const;
    PBR_INLINE T Length() const;

    bool HasNaNs() const
    {
        return isNaN(x) || isNaN(y);
    }


    T x, y;
};

using Vector2_t = Vector2<fp_t>;
template<typename T> using Vector2_arg = Vector2<T>&;


// ---------------------------------------
// ------------ CONSTRUCTORS -------------
// ---------------------------------------

template<typename T> PBR_CNSTEXPR
Vector2<T>::Vector2()
    : x(0), y(0)
{}

template<typename T> PBR_CNSTEXPR
Vector2<T>::Vector2(T value)
    : x(value), y(value)
{
    PBR_ASSERT(!HasNaNs())
}

template<typename T> PBR_CNSTEXPR
Vector2<T>::Vector2(T x, T y)
    : x(x), y(y)
{
    PBR_ASSERT(!HasNaNs())
}


// ---------------------------------------
// ---- COMPOUND ARITHMETIC OPERATORS ----
// ---------------------------------------

#define COMPOUND_OPERATOR_V(op)                                \
    template<typename T> PBR_CNSTEXPR PBR_INLINE               \
    Vector2<T>& Vector2<T>::operator op(const Vector2_arg v) { \
        PBR_ASSERT(!v.HasNaNs())                               \
        x op v.x; y op v.y;                                    \
        return *this;                                          \
    }

COMPOUND_OPERATOR_V(+=)
COMPOUND_OPERATOR_V(-=)


#define COMPOUND_OPERATOR_S(op)                           \
    template<typename T> PBR_CNSTEXPR PBR_INLINE          \
    Vector2<T>& Vector2<T>::operator op(const T scalar) { \
        x op scalar; y op scalar;                         \
        return *this;                                     \
    }

COMPOUND_OPERATOR_S(*=)

template<typename T> PBR_CNSTEXPR PBR_INLINE
Vector2<T>& Vector2<T>::operator/=(const T scalar)
{
    PBR_ASSERT(scalar != 0)

    if constexpr (std::numeric_limits<T>::is_iec559) {
        T inv = static_cast<T>(1) / scalar;
        x *= inv; y *= inv;
        return *this;
    }
    else {
        x /= scalar; y /= scalar;
        return *this;
    }
}


// ---------------------------------------
// ----- UNARY ARITHMETIC OPERATORS ------
// ---------------------------------------

template<typename T> PBR_CNSTEXPR PBR_INLINE
Vector2<T> operator-(const Vector2_arg<T> v)
{
    return Vector2<T>(-v.x, -v.y);
}


// ---------------------------------------
// ----- BINARY ARITHMETIC OPERATORS -----
// ---------------------------------------

#define BINARY_OPERATOR_VV(op)                                                 \
    template<typename T> PBR_CNSTEXPR PBR_INLINE                               \
    Vector2<T> operator op(const Vector2_arg<T> v1, const Vector2_arg<T> v2) { \
        PBR_ASSERT(!v2.HasNaNs())                                              \
        return Vector2<T>(v1.x op v2.x, v1.y op v2.y);                         \
    }

BINARY_OPERATOR_VV(+)
BINARY_OPERATOR_VV(-)


#define BINARY_OPERATOR_VS(op)                                       \
    template<typename T> PBR_CNSTEXPR PBR_INLINE                     \
    Vector2<T> operator op(const Vector2_arg<T> v, const T scalar) { \
        return Vector2<T>(v.x op scalar, v.y op scalar);             \
    }

BINARY_OPERATOR_VS(*)

template<typename T> PBR_CNSTEXPR PBR_INLINE
Vector2<T> operator/(const Vector2_arg<T> v, const T scalar)
{
    PBR_ASSERT(scalar != 0)

    if constexpr (std::numeric_limits<T>::is_iec559) {
        T inv = static_cast<T>(1) / scalar;
        return Vector2<T>(v.x * inv, v.y * inv);
    }
    else
        return Vector2<T>(v.x / scalar, v.y / scalar);
}


#define BINARY_OPERATOR_SV(op)                                       \
    template<typename T> PBR_CNSTEXPR PBR_INLINE                     \
    Vector2<T> operator op(const T scalar, const Vector2_arg<T> v) { \
        return Vector2<T>(v.x op scalar, v.y op scalar);             \
    }

BINARY_OPERATOR_SV(*)


#undef COMPOUND_OPERATOR_V
#undef COMPOUND_OPERATOR_S

#undef BINARY_OPERATOR_VV
#undef BINARY_OPERATOR_VS
#undef BINARY_OPERATOR_SV


// ---------------------------------------
// --------------- METHODS ---------------
// ---------------------------------------

template<typename T> PBR_CNSTEXPR PBR_INLINE
T Vector2<T>::LengthSquared() const
{
    return x * x + y * y;
}

template<typename T> PBR_INLINE
T Vector2<T>::Length() const
{
    return pbr::sqrt(LengthSquared());
}


// ---------------------------------------
// ---------- UTILITY FUNCTIONS ----------
// ---------------------------------------

template<typename T> PBR_INLINE
Vector2<T> Normalize(const Vector2_arg<T> v)
{
    return v / v.Length();
}

template<typename T> PBR_INLINE
Vector2<T> Abs(const Vector2_arg<T> v)
{
    return Vector2<T>(std::abs(v.x), std::abs(v.y));
}

template<typename T> PBR_CNSTEXPR PBR_INLINE
T Dot(const Vector2_arg<T> v1, const Vector2_arg<T> v2)
{
    PBR_ASSERT(!v1.HasNaNs() && !v2.HasNaNs())
    return v1.x * v2.x + v1.y * v2.y;
}

template<typename T> PBR_INLINE
T AbsDot(const Vector2_arg<T> v1, const Vector2_arg<T> v2)
{
    PBR_ASSERT(!v1.HasNaNs() && !v2.HasNaNs())
    return std::abs(Dot(v1, v2));
}


#pragma endregion Vector2


// ******************************************************************************
// ---------------------------------- VECTOR3 -----------------------------------
// ******************************************************************************

#pragma region Vector3

template<typename T> class Point3; // NOTE: Forward declaration for Vector3(Point3_arg<T> p) conversion constructor
template<typename T> using Point3_arg = Point3<T>&;

// TODO: Implement index operator[]
template<typename T>
struct Vector3
{
    using Vector3_arg = Vector3<T>&;

    PBR_CNSTEXPR Vector3();
    PBR_CNSTEXPR explicit Vector3(T value);
    PBR_CNSTEXPR explicit Vector3(T x, T y, T z);

    PBR_CNSTEXPR explicit Vector3(Point3_arg<T> p); // Conversion constructor


    PBR_CNSTEXPR PBR_INLINE Vector3<T>& operator+=(const Vector3_arg v);
    PBR_CNSTEXPR PBR_INLINE Vector3<T>& operator-=(const Vector3_arg v);

    PBR_CNSTEXPR PBR_INLINE Vector3<T>& operator*=(const T scalar);
    PBR_CNSTEXPR PBR_INLINE Vector3<T>& operator/=(const T scalar);

    friend auto operator<=>(const Vector3_arg<T>, const Vector3_arg<T>) = default;

    /*T operator[](i32 i) const
    {
        PBR_ASSERT(i >= 0 && i <= 2)
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }
    T& operator[](i32 i)
    {
        PBR_ASSERT(i >= 0 && i <= 2)
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }*/

    PBR_CNSTEXPR PBR_INLINE T LengthSquared() const;
    PBR_INLINE T Length() const;

    bool HasNaNs() const
    {
        return isNaN(x) || isNaN(y) || isNaN(z);
    }


    T x, y, z;
};

using Vector3_t = Vector3<fp_t>;
template<typename T> using Vector3_arg = Vector3<T>&;


// ---------------------------------------
// ------------ CONSTRUCTORS -------------
// ---------------------------------------

template<typename T> PBR_CNSTEXPR
Vector3<T>::Vector3()
    : x(0), y(0), z(0)
{}

template<typename T> PBR_CNSTEXPR
Vector3<T>::Vector3(T value)
    : x(value), y(value), z(value)
{
    PBR_ASSERT(!HasNaNs())
}

template<typename T> PBR_CNSTEXPR
Vector3<T>::Vector3(T x, T y, T z)
    : x(x), y(y), z(z)
{
    PBR_ASSERT(!HasNaNs())
}

template<typename T> PBR_CNSTEXPR
Vector3<T>::Vector3(Point3_arg<T> p)
    : x(p.x), y(p.y), z(p.z)
{
    PBR_ASSERT(!HasNaNs())
}


// ---------------------------------------
// ---- COMPOUND ARITHMETIC OPERATORS ----
// ---------------------------------------

#define COMPOUND_OPERATOR_V(op)                                \
    template<typename T> PBR_CNSTEXPR PBR_INLINE               \
    Vector3<T>& Vector3<T>::operator op(const Vector3_arg v) { \
        PBR_ASSERT(!v.HasNaNs())                               \
        x op v.x; y op v.y; z op v.z;                          \
        return *this;                                          \
    }

COMPOUND_OPERATOR_V(+=)
COMPOUND_OPERATOR_V(-=)

#define COMPOUND_OPERATOR_S(op)                           \
    template<typename T> PBR_CNSTEXPR PBR_INLINE          \
    Vector3<T>& Vector3<T>::operator op(const T scalar) { \
        x op scalar; y op scalar; z op scalar;            \
        return *this;                                     \
    }

COMPOUND_OPERATOR_S(*=)

template<typename T> PBR_CNSTEXPR PBR_INLINE
Vector3<T>& Vector3<T>::operator/=(const T scalar)
{
    PBR_ASSERT(scalar != 0)

    if constexpr (std::numeric_limits<T>::is_iec559) {
        T inv = static_cast<T>(1) / scalar;
        x *= inv; y *= inv; z *= inv;
        return *this;
    }
    else {
        x /= scalar; y /= scalar; z /= scalar;
        return *this;
    }
}


// ---------------------------------------
// ----- UNARY ARITHMETIC OPERATORS ------
// ---------------------------------------

template<typename T> PBR_CNSTEXPR PBR_INLINE
Vector3<T> operator-(const Vector3_arg<T> v)
{
    return Vector3<T>(-v.x, -v.y, -v.z);
}


// ---------------------------------------
// ----- BINARY ARITHMETIC OPERATORS -----
// ---------------------------------------

#define BINARY_OPERATOR_VV(op)                                                 \
    template<typename T> PBR_CNSTEXPR PBR_INLINE                               \
    Vector3<T> operator op(const Vector3_arg<T> v1, const Vector3_arg<T> v2) { \
        PBR_ASSERT(!v2.HasNaNs())                                              \
        return Vector3<T>(v1.x op v2.x, v1.y op v2.y, v1.z op v2.z);           \
    }

BINARY_OPERATOR_VV(+)
BINARY_OPERATOR_VV(-)
BINARY_OPERATOR_VV(*)


#define BINARY_OPERATOR_VS(op)                                          \
    template<typename T> PBR_CNSTEXPR PBR_INLINE                        \
    Vector3<T> operator op(const Vector3_arg<T> v, const T scalar) {    \
        return Vector3<T>(v.x op scalar, v.y op scalar, v.z op scalar); \
    }

BINARY_OPERATOR_VS(*)

template<typename T> PBR_CNSTEXPR PBR_INLINE
Vector3<T> operator/(const Vector3_arg<T> v, const T scalar)
{
    PBR_ASSERT(scalar != 0)

    if constexpr (std::numeric_limits<T>::is_iec559) {
        T inv = static_cast<T>(1) / scalar;
        return Vector3<T>(v.x * inv, v.y * inv, v.z * inv);
    }
    else
        return Vector3<T>(v.x / scalar, v.y / scalar, v.z / scalar);
}


#define BINARY_OPERATOR_SV(op)                                          \
    template<typename T> PBR_CNSTEXPR PBR_INLINE                        \
    Vector3<T> operator op(const T scalar, const Vector3_arg<T> v) {    \
        return Vector3<T>(v.x op scalar, v.y op scalar, v.z op scalar); \
    }

BINARY_OPERATOR_SV(*)


#undef COMPOUND_OPERATOR_V
#undef COMPOUND_OPERATOR_S

#undef BINARY_OPERATOR_VV
#undef BINARY_OPERATOR_VS
#undef BINARY_OPERATOR_SV


// ---------------------------------------
// --------------- METHODS ---------------
// ---------------------------------------

template<typename T> PBR_CNSTEXPR PBR_INLINE
T Vector3<T>::LengthSquared() const
{
    return x * x + y * y + z * z;
}

template<typename T> PBR_INLINE
T Vector3<T>::Length() const
{
    return pbr::sqrt(LengthSquared());
}


// ---------------------------------------
// ---------- UTILITY FUNCTIONS ----------
// ---------------------------------------

template<typename T> PBR_INLINE
Vector3<T> Normalize(const Vector3_arg<T> v)
{
    return v / v.Length();
}

template<typename T> PBR_INLINE
Vector3<T> Abs(const Vector3_arg<T> v)
{
    return Vector3<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z));
}

template<typename T> PBR_CNSTEXPR PBR_INLINE
T Dot(const Vector3_arg<T> v1, const Vector3_arg<T> v2)
{
    PBR_ASSERT(!v1.HasNaNs() && !v2.HasNaNs())
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

template<typename T> PBR_INLINE
T AbsDot(const Vector3_arg<T> v1, const Vector3_arg<T> v2)
{
    PBR_ASSERT(!v1.HasNaNs() && !v2.HasNaNs())
    return std::abs(Dot(v1, v2));
}

template<typename T> PBR_CNSTEXPR PBR_INLINE
Vector3<T> Cross(const Vector3_arg<T> v1, const Vector3_arg<T> v2)
{
    PBR_ASSERT(!n1.HasNaNs() && !n2.HasNaNs())
    if constexpr (std::is_same<T, f64>()) {
        return Vector3<T>(v1.y * v2.z - v1.z * v2.y,
                          v1.z * v2.x - v1.x * v2.z,
                          v1.x * v2.y - v1.y * v2.x);
    }
    else {
        f64 v1x = v1.x, v1y = v1.y, v1z = v1.z;
        f64 v2x = v2.x, v2y = v2.y, v2z = v2.z;
        return Vector3<T>(v1y * v2z - v1z * v2y,
                          v1z * v2x - v1x * v2z,
                          v1x * v2y - v1y * v2x);
    }
}

template<typename T> PBR_CNSTEXPR PBR_INLINE
Vector3<T> Min(const Vector3_arg<T> v1, const Vector3_arg<T> v2)
{
    return Vector3<T>(std::min(v1.x, v2.x),
                      std::min(v1.y, v2.y),
                      std::min(v1.z, v2.z));
}

template<typename T> PBR_CNSTEXPR PBR_INLINE
Vector3<T> Max(const Vector3_arg<T> v1, const Vector3_arg<T> v2)
{
    return Vector3<T>(std::max(v1.x, v2.x),
                      std::max(v1.y, v2.y),
                      std::max(v1.z, v2.z));
}

template<typename T> PBR_CNSTEXPR PBR_INLINE
T MinComponent(const Vector3_arg<T> v)
{
    return pbr::Min3(v.x, v.y, v.z);
}

template<typename T> PBR_CNSTEXPR PBR_INLINE
T MaxComponent(const Vector3_arg<T> v)
{
    return pbr::Max3(v.x, v.y, v.z);
}

// NOTE: This generates ugly assembly, and I don't know if there is a way to improve it. It can be done with only 2 comparison, but idk if it's better.
// Returns index of max value in the vector, eg. Vector3(1,2,3) -> 0, Vector3(3,2,1) -> 2.
template<typename T> PBR_CNSTEXPR PBR_INLINE
i32 MaxDimension(const Vector3_arg<T> v)
{
    if(v.x > v.y && v.x > v.z) return 0;
    if(v.y > v.z) return 1;
    return 2;
}

template<typename T> PBR_CNSTEXPR PBR_INLINE
Vector3<T> Permute(const Vector3_arg<T> v, i32 x, i32 y, i32 z)
{
   return Vector3<T>(v[x], v[y], v[z]);
}

template<typename T> PBR_INLINE
Vector3<T> CoordinateSystem(const Vector3_arg<T> v1, Vector3<T>& v2, Vector3<T>& v3)
{
    if (std::abs(v1.x) > std::abs(v1.y))
        v2 = Vector3<T>(-v1.z, 0, v1.x) / pbr::sqrt(v1.x * v1.x + v1.z * v1.z);
    else
        v2 = Vector3<T>(0, v1.z, -v1.y) / pbr::sqrt(v1.y * v1.y + v1.z * v1.z);

    v3 = Cross(v1, v2);
}


#pragma endregion Vector3


// ******************************************************************************
// ----------------------------------- POINT2 -----------------------------------
// ******************************************************************************

#pragma region Point2

template<typename T>
struct Point2
{
    using Point2_arg = Point2<T>&;

    PBR_CNSTEXPR Point2();
    PBR_CNSTEXPR explicit Point2(T value);
    PBR_CNSTEXPR explicit Point2(T x, T y);


    PBR_CNSTEXPR PBR_INLINE Point2<T>& operator+=(const Point2_arg v);
    PBR_CNSTEXPR PBR_INLINE Point2<T>& operator-=(const Point2_arg v);

    PBR_CNSTEXPR PBR_INLINE Point2<T>& operator*=(const T scalar);
    PBR_CNSTEXPR PBR_INLINE Point2<T>& operator/=(const T scalar);

    friend auto operator<=>(const Point2<T>, const Point2<T>) = default;


    PBR_CNSTEXPR PBR_INLINE T LengthSquared() const;
    PBR_INLINE T Length() const;

    bool HasNaNs() const
    {
        return isNaN(x) || isNaN(y);
    }


    T x, y;
};

using Point2_t = Point2<fp_t>;
template<typename T> using Point2_arg = Point2<T>&;


// ---------------------------------------
// ------------ CONSTRUCTORS -------------
// ---------------------------------------

template<typename T> PBR_CNSTEXPR
Point2<T>::Point2()
    : x(0), y(0)
{}

template<typename T> PBR_CNSTEXPR
Point2<T>::Point2(T value)
    : x(value), y(value)
{
    PBR_ASSERT(!HasNaNs())
}

template<typename T> PBR_CNSTEXPR
Point2<T>::Point2(T x, T y)
    : x(x), y(y)
{
    PBR_ASSERT(!HasNaNs())
}


// ---------------------------------------
// ---- COMPOUND ARITHMETIC OPERATORS ----
// ---------------------------------------

#define COMPOUND_OPERATOR_P(op)                             \
    template<typename T> PBR_CNSTEXPR PBR_INLINE            \
    Point2<T>& Point2<T>::operator op(const Point2_arg p) { \
        PBR_ASSERT(!p.HasNaNs())                            \
        x op p.x; y op p.y;                                 \
        return *this;                                       \
    }

COMPOUND_OPERATOR_P(+=)


#define COMPOUND_OPERATOR_S(op)                         \
    template<typename T> PBR_CNSTEXPR PBR_INLINE        \
    Point2<T>& Point2<T>::operator op(const T scalar) { \
        x op scalar; y op scalar;                       \
        return *this;                                   \
    }

COMPOUND_OPERATOR_S(*=)

template<typename T> PBR_CNSTEXPR PBR_INLINE
Point2<T>& Point2<T>::operator/=(const T scalar)
{
    PBR_ASSERT(scalar != 0)

    if constexpr (std::numeric_limits<T>::is_iec559) {
        T inv = static_cast<T>(1) / scalar;
        x *= inv; y *= inv;
        return *this;
    }
    else {
        x /= scalar; y /= scalar;
        return *this;
    }
}


// ---------------------------------------
// ----- UNARY ARITHMETIC OPERATORS ------
// ---------------------------------------

template<typename T> PBR_CNSTEXPR PBR_INLINE
Point2<T> operator-(const Point2_arg<T> p)
{
    return Point2<T>(-p.x, -p.y);
}


// ---------------------------------------
// ----- BINARY ARITHMETIC OPERATORS -----
// ---------------------------------------

template<typename T> PBR_CNSTEXPR PBR_INLINE
Point2<T> operator+(const Point2_arg<T> p1, const Point2_arg<T> p2)
{
    PBR_ASSERT(!p2.HasNaNs())
    return Point2<T>(p1.x + p2.x, p1.y + p2.y);
}

template<typename T> PBR_CNSTEXPR PBR_INLINE
Vector2<T> operator-(const Point2_arg<T> p1, const Point2_arg<T> p2)
{
    PBR_ASSERT(!p2.HasNaNs())
    return Point2<T>(p1.x - p2.x, p1.y - p2.y);
}


#define BINARY_OPERATOR_PS(op)                                     \
    template<typename T> PBR_CNSTEXPR PBR_INLINE                   \
    Point2<T> operator op(const Point2_arg<T> p, const T scalar) { \
        return Point2<T>(p.x op scalar, p.y op scalar);            \
    }

BINARY_OPERATOR_PS(*)

template<typename T> PBR_CNSTEXPR PBR_INLINE
Point2<T> operator/(const Point2_arg<T> p, const T scalar)
{
    PBR_ASSERT(scalar != 0)

    if constexpr (std::numeric_limits<T>::is_iec559) {
        T inv = static_cast<T>(1) / scalar;
        return Point2<T>(p.x * inv, p.y * inv);
    }
    else
        return Point2<T>(p.x / scalar, p.y / scalar);
}


#define BINARY_OPERATOR_SP(op)                                     \
    template<typename T> PBR_CNSTEXPR PBR_INLINE                   \
    Point2<T> operator op(const T scalar, const Point2_arg<T> p) { \
        return Point2<T>(p.x op scalar, p.y op scalar);            \
    }

BINARY_OPERATOR_SP(*)


#undef COMPOUND_OPERATOR_P
#undef COMPOUND_OPERATOR_S

#undef BINARY_OPERATOR_PS
#undef BINARY_OPERATOR_SP


#pragma endregion Point2


// ******************************************************************************
// ----------------------------------- POINT3 -----------------------------------
// ******************************************************************************

#pragma region Point3

template<typename T>
struct Point3
{
    using Point3_arg = Point3<T>&;

    PBR_CNSTEXPR Point3();
    PBR_CNSTEXPR explicit Point3(T value);
    PBR_CNSTEXPR explicit Point3(T x, T y, T z);

    PBR_CNSTEXPR explicit Point3(const Vector3_arg<T> v); // Conversion constructor


    PBR_CNSTEXPR PBR_INLINE Point3<T>& operator+=(const Point3_arg p);

    PBR_CNSTEXPR PBR_INLINE Point3<T>& operator*=(const T scalar);
    PBR_CNSTEXPR PBR_INLINE Point3<T>& operator/=(const T scalar);

    PBR_CNSTEXPR PBR_INLINE Point3<T>& operator+=(const Vector3_arg<T> p);
    PBR_CNSTEXPR PBR_INLINE Point3<T>& operator-=(const Vector3_arg<T> p);


    bool HasNaNs() const
    {
        return isNaN(x) || isNaN(y) || isNaN(z);
    }


    T x, y, z;
};

using Point3_t = Point3<fp_t>;
// NOTE: Declared before Vector3, cause of fockin conversion constructor
//template<typename T> using Point3_arg = Point3<T>&;


// ---------------------------------------
// ------------ CONSTRUCTORS -------------
// ---------------------------------------

template<typename T> PBR_CNSTEXPR
Point3<T>::Point3() : x(0), y(0), z(0)
{}

template<typename T> PBR_CNSTEXPR
Point3<T>::Point3(T value)
    : x(value), y(value), z(value)
{
    PBR_ASSERT(!HasNaNs())
}

template<typename T> PBR_CNSTEXPR
Point3<T>::Point3(T x, T y, T z)
    : x(x), y(y), z(z)
{
    PBR_ASSERT(!HasNaNs())
}

template<typename T> PBR_CNSTEXPR
Point3<T>::Point3(const Vector3_arg<T> v)
    : x(v.x), y(v.y), z(v.z)
{
    PBR_ASSERT(!HasNaNs())
}

// ---------------------------------------
// ---- COMPOUND ARITHMETIC OPERATORS ----
// ---------------------------------------

#define COMPOUND_OPERATOR_P(op)                             \
    template<typename T> PBR_CNSTEXPR PBR_INLINE            \
    Point3<T>& Point3<T>::operator op(const Point3_arg p) { \
        x op p.x; y op p.y; z op p.z;                       \
        return *this;                                       \
    }

COMPOUND_OPERATOR_P(+=)


#define COMPOUND_OPERATOR_S(op)                         \
    template<typename T> PBR_CNSTEXPR PBR_INLINE        \
    Point3<T>& Point3<T>::operator op(const T scalar) { \
        x op scalar; y op scalar; z op scalar;          \
        return *this;                                   \
    }

COMPOUND_OPERATOR_S(*=)

template<typename T> PBR_CNSTEXPR PBR_INLINE
Point3<T>& Point3<T>::operator/=(const T scalar)
{
    PBR_ASSERT(scalar != 0)

    if constexpr (std::numeric_limits<T>::is_iec559) {
        T inv = static_cast<T>(1) / scalar;
        x *= inv; y *= inv; z *= inv;
        return *this;
    }
    else {
        x /= scalar; y /= scalar; z /= scalar;
        return *this;
    }
}


// ---------------------------------------
// ----- UNARY ARITHMETIC OPERATORS ------
// ---------------------------------------

template<typename T> PBR_CNSTEXPR PBR_INLINE
Point3<T> operator-(const Point3_arg<T> p)
{
    return Point3<T>(-p.x, -p.y, -p.z);
}


// ---------------------------------------
// ----- BINARY ARITHMETIC OPERATORS -----
// ---------------------------------------

#define BINARY_OPERATOR_PP(op)                                              \
    template<typename T> PBR_CNSTEXPR PBR_INLINE                            \
    Point3<T> operator op(const Point3_arg<T> p1, const Point3_arg<T> p2) { \
        PBR_ASSERT(!p2.HasNaNs())                                           \
        return Point3<T>(p1.x op p2.x, p1.y op p2.y, p1.z op p2.z);         \
    }

BINARY_OPERATOR_PP(+)

template<typename T> PBR_CNSTEXPR PBR_INLINE
Vector3<T> operator-(const Point3_arg<T> p1, const Point3_arg<T> p2) {
    PBR_ASSERT(!p2.HasNaNs())
    return Vector3<T>(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z);
}


#define BINARY_OPERATOR_PS(op)                                         \
    template<typename T> PBR_CNSTEXPR PBR_INLINE                       \
    Point3<T> operator op(const Point3_arg<T> p, const T scalar) {     \
        return Point3<T>(p.x op scalar, p.y op scalar, p.z op scalar); \
    }

BINARY_OPERATOR_PS(*)

template<typename T> PBR_CNSTEXPR PBR_INLINE
Point3<T>& operator/(const Point3_arg<T> p, const T scalar)
{
    PBR_ASSERT(scalar != 0)

    if constexpr (std::numeric_limits<T>::is_iec559) {
        T inv = static_cast<T>(1) / scalar;
        return Point3<T>(p.x * inv, p.y * inv, p.z * inv);
    }
    else
        return Point3<T>(p.x / scalar, p.y / scalar, p.z / scalar);
}


#define BINARY_OPERATOR_SP(op)                                         \
    template<typename T> PBR_CNSTEXPR PBR_INLINE                       \
    Point3<T> operator op(const T scalar, const Point3_arg<T> p) {     \
        PBR_ASSERT(!p.HasNaNs())                                       \
        return Point3<T>(p.x op scalar, p.y op scalar, p.z op scalar); \
    }

BINARY_OPERATOR_SP(*)


#define BINARY_OPERATOR_PV(op)                                             \
    template<typename T> PBR_CNSTEXPR PBR_INLINE                           \
    Point3<T> operator op(const Point3_arg<T> p, const Vector3_arg<T> v) { \
        PBR_ASSERT(!v.HasNaNs())                                           \
        return Point3<T>(p.x op v.x, p.y op v.y, p.z op v.z);              \
    }

BINARY_OPERATOR_PV(+)
BINARY_OPERATOR_PV(-)


#undef COMPOUND_OPERATOR_P
#undef COMPOUND_OPERATOR_S

#undef BINARY_OPERATOR_PP
#undef BINARY_OPERATOR_PS
#undef BINARY_OPERATOR_SP

#undef BINARY_OPERATOR_PV


// ---------------------------------------
// ---------- UTILITY FUNCTIONS ----------
// ---------------------------------------

template<typename Float> PBR_CNSTEXPR PBR_INLINE
Point3<Float> Lerp(Float t, const Point3_arg<Float> p1, const Point3_arg<Float> p2)
{
    static_assert(std::numeric_limits<Float>::is_iec559);
    return p1 + (p2 - p1) * t;
}

template<typename T> PBR_CNSTEXPR PBR_INLINE
Point3<T> Min(const Point3_arg<T> p1, const Point3_arg<T> p2)
{
    return Point3<T>(std::min(p1.x, p2.x),
                     std::min(p1.y, p2.y),
                     std::min(p1.z, p2.z));
}

template<typename T> PBR_CNSTEXPR PBR_INLINE
Point3<T> Max(const Point3_arg<T> p1, const Point3_arg<T> p2)
{
    return Point3<T>(std::max(p1.x, p2.x),
                     std::max(p1.y, p2.y),
                     std::max(p1.z, p2.z));
}

template<typename T> PBR_INLINE
Point3<T> Floor(const Point3_arg<T> p)
{
    return Point3<T>(pbr::floor(p.x), pbr::floor(p.y), pbr::floor(p.z));
}

template<typename T> PBR_INLINE
Point3<T> Ceil(const Point3_arg<T> p)
{
    return Point3<T>(pbr::ceil(p.x), pbr::ceil(p.y), pbr::ceil(p.z));
}

template<typename T> PBR_INLINE
Point3<T> Abs(const Point3_arg<T> p)
{
    return Point3<T>(std::abs(p.x), std::abs(p.y), std::abs(p.z));
}

template<typename T> PBR_CNSTEXPR PBR_INLINE
Point3_arg<T> Permute(const Point3_arg<T> p, i32 x, i32 y, i32 z)
{
   return Point3_arg<T>(p[x], p[y], p[z]);
}

template<typename T> PBR_CNSTEXPR PBR_INLINE
T Distance(const Point3_arg<T> p1, const Point3_arg<T> p2) {
    return (p1 - p2).Length();
}


#pragma endregion Point3


// ******************************************************************************
// ---------------------------------- NORMAL3 -----------------------------------
// ******************************************************************************

#pragma region Normal3

template<typename T>
struct Normal3
{
    using Normal3_arg = Normal3<T>&;

    PBR_CNSTEXPR Normal3();
    PBR_CNSTEXPR explicit Normal3(T value);
    PBR_CNSTEXPR explicit Normal3(T x, T y, T z);

    PBR_CNSTEXPR explicit Normal3(const Vector3_arg<T> v); // Conversion constructor


    PBR_CNSTEXPR PBR_INLINE T LengthSquared() const;
    PBR_INLINE T Length() const;

    //PBR_INLINE T Vector3<T>::Abs() const;

    PBR_CNSTEXPR PBR_INLINE Normal3<T>& operator+=(const Normal3_arg v);
    PBR_CNSTEXPR PBR_INLINE Normal3<T>& operator-=(const Normal3_arg v);

    PBR_CNSTEXPR PBR_INLINE Normal3<T>& operator*=(const T scalar);
    PBR_CNSTEXPR PBR_INLINE Normal3<T>& operator/=(const T scalar);


    bool HasNaNs() const
    {
        return isNaN(x) || isNaN(y) || isNaN(z);
    }


    T x, y, z;
};

using Normal3_t = Normal3<fp_t>;
template<typename T> using Normal3_arg = Normal3<T>&;


// ---------------------------------------
// ------------ CONSTRUCTORS -------------
// ---------------------------------------

template<typename T> PBR_CNSTEXPR
Normal3<T>::Normal3()
    : x(0), y(0), z(0)
{}

template<typename T> PBR_CNSTEXPR
Normal3<T>::Normal3(T value)
    : x(value), y(value), z(value)
{
    PBR_ASSERT(!HasNaNs())
}

template<typename T> PBR_CNSTEXPR
Normal3<T>::Normal3(T x, T y, T z)
    : x(x), y(y), z(z)
{
    PBR_ASSERT(!HasNaNs())
}

template<typename T> PBR_CNSTEXPR
Normal3<T>::Normal3(const Vector3_arg<T> v)
    : x(v.x), y(v.y), z(v.z)
{
    PBR_ASSERT(!HasNaNs())
}


// ---------------------------------------
// ---- COMPOUND ARITHMETIC OPERATORS ----
// ---------------------------------------

#define COMPOUND_OPERATOR_N(op)                                \
    template<typename T> PBR_CNSTEXPR PBR_INLINE               \
    Normal3<T>& Normal3<T>::operator op(const Normal3_arg n) { \
        PBR_ASSERT(!n.HasNaNs())                               \
        x op n.x; y op n.y; z op n.z;                          \
        return *this;                                          \
    }

COMPOUND_OPERATOR_N(+=)
COMPOUND_OPERATOR_N(-=)


#define COMPOUND_OPERATOR_S(op)                           \
    template<typename T> PBR_CNSTEXPR PBR_INLINE          \
    Normal3<T>& Normal3<T>::operator op(const T scalar) { \
        x op scalar; y op scalar; z op scalar;            \
        return *this;                                     \
    }

COMPOUND_OPERATOR_S(*=)

template<typename T> PBR_CNSTEXPR PBR_INLINE
Normal3<T>& Normal3<T>::operator/=(const T scalar)
{
    PBR_ASSERT(scalar != 0)

    if constexpr (std::numeric_limits<T>::is_iec559) {
        T inv = static_cast<T>(1) / scalar;
        x *= inv; y *= inv; z *= inv;
        return *this;
    }
    else {
        x /= scalar; y /= scalar; z /= scalar;
        return *this;
    }
}


// ---------------------------------------
// ----- UNARY ARITHMETIC OPERATORS ------
// ---------------------------------------

template<typename T> PBR_CNSTEXPR PBR_INLINE
Normal3<T> operator-(const Normal3_arg<T> n)
{
    return Normal3<T>(-n.x, -n.y, -n.z);
}


// ---------------------------------------
// ----- BINARY ARITHMETIC OPERATORS -----
// ---------------------------------------

#define BINARY_OPERATOR_NN(op)                                                 \
    template<typename T> PBR_CNSTEXPR PBR_INLINE                               \
    Normal3<T> operator op(const Normal3_arg<T> n1, const Normal3_arg<T> n2) { \
        PBR_ASSERT(!n2.HasNaNs())                                              \
        return Normal3<T>(n1.x op n2.x, n1.y op n2.y, n1.z op n2.z);           \
    }

BINARY_OPERATOR_NN(+)
BINARY_OPERATOR_NN(-)
BINARY_OPERATOR_NN(*)


#define BINARY_OPERATOR_NS(op)                                          \
    template<typename T> PBR_CNSTEXPR PBR_INLINE                        \
    Normal3<T> operator op(const Normal3_arg<T> n, const T scalar) {    \
        return Normal3<T>(n.x op scalar, n.y op scalar, n.z op scalar); \
    }

//BINARY_OPERATOR_VS(+)
//BINARY_OPERATOR_VS(-)
//BINARY_OPERATOR_VS(*)

template<typename T> PBR_CNSTEXPR PBR_INLINE
Vector3<T> operator/(const Normal3_arg<T> n, const T scalar)
{
    PBR_ASSERT(scalar != 0)

    if constexpr (std::numeric_limits<T>::is_iec559) {
        T inv = static_cast<T>(1) / scalar;
        return Normal3<T>(n.x * inv, n.y * inv, n.z * inv);
    }
    else
        return Normal3<T>(n.x / scalar, n.y / scalar, n.z / scalar);
}


#define BINARY_OPERATOR_SN(op)                                          \
    template<typename T> PBR_CNSTEXPR PBR_INLINE                        \
    Normal3<T> operator op(const T scalar, const Normal3_arg<T> n) {    \
        return Normal3<T>(n.x op scalar, n.y op scalar, n.z op scalar); \
    }

//BINARY_OPERATOR_SV(+)
//BINARY_OPERATOR_SV(-)
//BINARY_OPERATOR_SV(*)


#undef COMPOUND_OPERATOR_N
#undef COMPOUND_OPERATOR_S

#undef BINARY_OPERATOR_NN
#undef BINARY_OPERATOR_NS
#undef BINARY_OPERATOR_SN


// ---------------------------------------
// --------------- METHODS ---------------
// ---------------------------------------

template<typename T> PBR_CNSTEXPR PBR_INLINE
T Normal3<T>::LengthSquared() const
{
    return x * x + y * y + z * z;
}

template<typename T> PBR_INLINE
T Normal3<T>::Length() const
{
    return pbr::sqrt(LengthSquared());
}


// ---------------------------------------
// ---------- UTILITY FUNCTIONS ----------
// ---------------------------------------

template<typename T> PBR_INLINE
Normal3<T> Normalize(const Normal3_arg<T> n)
{
    return n / n.Length();
}

template<typename T> PBR_INLINE
T Abs(const Normal3_arg<T> p)
{
    return Normal3<T>(std::abs(p.x), std::abs(p.y), std::abs(p.z));
}

template<typename T> PBR_CNSTEXPR PBR_INLINE
T Dot(const Normal3_arg<T> n1, const Normal3_arg<T> n2)
{
    PBR_ASSERT(!n1.HasNaNs() && !n2.HasNaNs())
    return n1.x * n2.x + n1.y * n2.y + n1.z * n2.z;
}

template<typename T> PBR_INLINE
T AbsDot(const Normal3_arg<T> n1, const Normal3_arg<T> n2)
{
    PBR_ASSERT(!n1.HasNaNs() && !n2.HasNaNs())
    return std::abs(Dot(n1, n2));
}

template<typename T> PBR_CNSTEXPR PBR_INLINE
Normal3<T> Cross(const Normal3_arg<T> n1, const Normal3_arg<T> n2)
{
    PBR_ASSERT(!n1.HasNaNs() && !n2.HasNaNs())
    if constexpr (std::is_same<T, f64>()) {
        return Normal3<T>(n1.y * n2.z - n1.z * n2.y,
                          n1.z * n2.x - n1.x * n2.z,
                          n1.x * n2.y - n1.y * n2.x);
    }
    else {
        f64 n1x = n1.x, n1y = n1.y, n1z = n1.z;
        f64 n2x = n2.x, n2y = n2.y, n2z = n2.z;
        return Normal3<T>(n1y * n2z - n1z * n2y,
                          n1z * n2x - n1x * n2z,
                          n1x * n2y - n1y * n2x);
    }
}

template<typename T> PBR_CNSTEXPR PBR_INLINE
Normal3<T> Min(const Normal3_arg<T> n1, const Normal3_arg<T> n2)
{
    return Normal3<T>(std::min(n1.x, n2.x),
                      std::min(n1.y, n2.y),
                      std::min(n1.z, n2.z));
}

template<typename T> PBR_CNSTEXPR PBR_INLINE
Normal3<T> Max(const Normal3_arg<T> n1, const Normal3_arg<T> n2)
{
    return Normal3<T>(std::max(n1.x, n2.x),
                      std::max(n1.y, n2.y),
                      std::max(n1.z, n2.z));
}

template<typename T> PBR_CNSTEXPR PBR_INLINE
Normal3<T> FaceForward(const Normal3_arg<T> n1, const Normal3_arg<T> n2)
{
    return (Dot(n1,n2) < static_cast<T>(0)) ? -n1 : n1;
}

#pragma endregion Normal3


// ******************************************************************************
// ------------------------------------ RAY -------------------------------------
// ******************************************************************************

#pragma region Ray

// TODO: Medium not implemented.
struct Ray
{
    PBR_CNSTEXPR Ray();
    PBR_CNSTEXPR explicit Ray(const Point3_arg<fp_t> origin,
                              const Vector3_arg<fp_t> direction,
                              fp_t tMax = constants::infinity,
                              fp_t time = static_cast<fp_t>(0)
                              /*const Medium *medium = nullptr*/);


    PBR_CNSTEXPR PBR_INLINE Point3_t operator()(fp_t t) const;


    Point3_t origin;
    Vector3_t direction;
    //const Medium *medium;
    fp_t tMax; // DIFFERENCE: Marked as mutable.
    fp_t time;
};

using Ray_arg = Ray&;


// ---------------------------------------
// ------------ CONSTRUCTORS -------------
// ---------------------------------------

PBR_CNSTEXPR
Ray::Ray()
    : tMax(constants::infinity)
    , time(static_cast<fp_t>(0))
    //, medium(nullptr)
{}

PBR_CNSTEXPR
Ray::Ray(const Point3_arg<fp_t> origin,
         const Vector3_arg<fp_t> direction,
         fp_t tMax, /* = constants::infinity */
         fp_t time /* = static_cast<fp_t>(0)) */
         /* const Medium *medium /* = nullptr */)
    : origin(origin)
    , direction(direction)
    , tMax(tMax)
    , time(time)
    //, medium(medium)
{}


// ---------------------------------------
// ------- FUNCTION CALL OPERATORS -------
// ---------------------------------------

PBR_CNSTEXPR PBR_INLINE
Point3_t Ray::operator()(fp_t t) const
{
    return origin + direction * t;
}

#pragma endregion Ray


// ******************************************************************************
// ------------------------------ RAYDIFFERENTIAL -------------------------------
// ******************************************************************************

#pragma region RayDifferential

struct RayDifferential : public Ray
{
    //using Vector3_arg = Vector3<T>&;

    PBR_CNSTEXPR RayDifferential();
    PBR_CNSTEXPR explicit RayDifferential(const Point3_arg<fp_t> origin,
                                          const Vector3_arg<fp_t> direction,
                                          fp_t tMax = constants::infinity,
                                          fp_t time = static_cast<fp_t>(0));
    PBR_CNSTEXPR explicit RayDifferential(const Ray_arg ray);


    PBR_CNSTEXPR void ScaleDifferentials(fp_t scale);


    Point3_t rxOrigin, ryOrigin;
    Vector3_t rxDirection, ryDirection;
    bool hasDifferentials;
};

using RayDifferential_arg = RayDifferential&;


// ---------------------------------------
// ------------ CONSTRUCTORS -------------
// ---------------------------------------

PBR_CNSTEXPR
RayDifferential::RayDifferential()
    : hasDifferentials(false)
{}

PBR_CNSTEXPR
RayDifferential::RayDifferential(const Point3_arg<fp_t> origin,
                                 const Vector3_arg<fp_t> direction,
                                 fp_t tMax, /* = constants::infinity*/
                                 fp_t time /* = static_cast<fp_t>(0)*/)
    : Ray(origin, direction, tMax, time)
    , hasDifferentials(false)
{}

PBR_CNSTEXPR
RayDifferential::RayDifferential(const Ray_arg ray)
    : Ray(ray)
    , hasDifferentials(false)
{}


// ---------------------------------------
// --------------- METHODS ---------------
// ---------------------------------------

// NOTE: possible optimization ?
// r = o + rx*s - o*s -> r = rx*s - o*(s-1)
PBR_CNSTEXPR
void RayDifferential::ScaleDifferentials(fp_t scale)
{
    rxOrigin = origin + (rxOrigin - origin) * scale;
    ryOrigin = origin + (ryOrigin - origin) * scale;
    rxDirection = direction + (rxDirection - direction) * scale;
    ryDirection = direction + (ryDirection - direction) * scale;
}


#pragma endregion RayDifferential


// ******************************************************************************
// ---------------------------------- BOUNDS3 -----------------------------------
// ******************************************************************************

#pragma region Bounds3

// TODO: May be Bounds3 doesn't nedd to be templated, same as Ray
template<typename T>
struct Bounds3
{
    //using Bounds3_arg = Bounds3<T>&;

    PBR_CNSTEXPR Bounds3();

    PBR_CNSTEXPR explicit Bounds3(const Point3_arg<T> p);
    PBR_CNSTEXPR explicit Bounds3(const Point3_arg<T> p1, const Point3_arg<T> p2);

    PBR_CNSTEXPR PBR_INLINE Vector3<T> Diagonal() const;
    PBR_CNSTEXPR PBR_INLINE Point3<T> Corner(i32 corner) const;

    PBR_CNSTEXPR PBR_INLINE T SurfaceArea() const;
    PBR_CNSTEXPR PBR_INLINE T Volume() const;

    // Returns the index of which of the three axes is longest, 0->x, 1->y, 2->z
    PBR_CNSTEXPR PBR_INLINE i32 MaximumExtent() const;

    // NOTE: questionable method
    // Lineary interpolates between the corners of the box by the given amount in each dimension
    PBR_CNSTEXPR PBR_INLINE Point3<T> Lerp(const Point3_arg<T> t) const;

    PBR_CNSTEXPR PBR_INLINE Vector3<T> Offset(const Point3_arg<T> p) const;

    PBR_CNSTEXPR PBR_INLINE void BoundingSphere(Point3<T> &center, fp_t &radius) const;

    // TODO: std::optional<fp_t,fp_t> ? And do i really need to pass by pointer ?
    // Checks for a ray-box intersection and returns two values of the intersection, if any
    PBR_CNSTEXPR PBR_INLINE bool IntersectP(const Ray_arg ray,
                                            fp_t *out_hit0 = nullptr,
                                            fp_t *out_hit1 = nullptr) const;
    PBR_CNSTEXPR PBR_INLINE bool IntersectP(const Ray_arg ray,
                                            const Vector3_arg<T> invDir,
                                            const i32 dirIsNeg[3]) const;


    Point3<T> pMin, pMax;
};

using Bounds3_t = Bounds3<fp_t>;
template<typename T> using Bounds3_arg = Bounds3<T>&;


// ---------------------------------------
// ------------ CONSTRUCTORS -------------
// ---------------------------------------

template<typename T> PBR_CNSTEXPR
Bounds3<T>::Bounds3()
{
    pMin = Point3<T>(std::numeric_limits<T>::max());
    pMax = Point3<T>(std::numeric_limits<T>::lowest());
}

template<typename T> PBR_CNSTEXPR
Bounds3<T>::Bounds3(const Point3_arg<T> p)
    : pMin(p), pMax(p)
{}

// NOTE: this constructor might be inefficient, cause of somitetimes useless Min, Max
// NOTE: May be I need a function MinMax(_1, _2) similar to std::minmax().
template<typename T> PBR_CNSTEXPR
Bounds3<T>::Bounds3(const Point3_arg<T> p1, const Point3_arg<T> p2)
    : pMin(Min(p1, p2))
    , pMax(Max(p1, p2))
{}


// ---------------------------------------
// --------------- METHODS ---------------
// ---------------------------------------

template<typename T> PBR_CNSTEXPR PBR_INLINE
Vector3<T> Bounds3<T>::Diagonal() const
{
    return pMax - pMin;
}

template<typename T> PBR_CNSTEXPR PBR_INLINE
T Bounds3<T>::SurfaceArea() const
{
    Vector3<T> d = Diagonal();
    return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
}

template<typename T> PBR_CNSTEXPR PBR_INLINE
T Bounds3<T>::Volume() const
{
    Vector3<T> d = Diagonal();
    return d.x * d.y * d.z;
}

// TODO: rework if I add operator[] to Bounds3 class
template<typename T> PBR_CNSTEXPR PBR_INLINE
Point3<T> Bounds3<T>::Corner(const i32 corner) const
{
    PBR_ASSERT(corner >= 0 && corner < 8)

    T x,y,z;
    x = (corner & 1 == 0) ? pMin.x : pMax.x;
    y = (corner & 2 == 0) ? pMin.y : pMax.y;
    z = (corner & 4 == 0) ? pMin.z : pMax.z;
    return Point3<T>(x, y, z);
}

template<typename T> PBR_CNSTEXPR
i32 Bounds3<T>::MaximumExtent() const
{
    Vector3<T> d = Diagonal();
    // NOTE: why '>' and not '>='
    if (d.x > d.y && d.x > d.z)
        return 0;
    if (d.y > d.z)
        return 1;
    return 2;
}

template<typename T> PBR_CNSTEXPR PBR_INLINE
Point3<T> Bounds3<T>::Lerp(const Point3_arg<T> t) const
{
    return Point3<T>(pbr::Lerp(t.x, pMin.x, pMax.x),
                     pbr::Lerp(t.y, pMin.y, pMax.y),
                     pbr::Lerp(t.z, pMin.z, pMax.z));
}

// TODO: in theory useless 'if' checks, but as i understand it's because of
// Bounds3(const Point3_arg<T> p) constructor
template<typename T> PBR_CNSTEXPR PBR_INLINE
Vector3<T> Bounds3<T>::Offset(const Point3_arg<T> p) const
{
    Vector3<T> o = p - pMin;
    if (pMax.x > pMin.x) o.x /= pMax.x - pMin.x;
    if (pMax.y > pMin.y) o.y /= pMax.y - pMin.y;
    if (pMax.z > pMin.z) o.z /= pMax.z - pMin.z;
    return o;
}

// TODO: probably should return pair, instead of returning through reference
template<typename T> PBR_CNSTEXPR PBR_INLINE
void Bounds3<T>::BoundingSphere(Point3<T> &center, fp_t &radius) const
{
    center = (pMin + pMax) / 2;
    radius = Inside(center, *this) ? Distance(center, pMax) : static_cast<fp_t>(0);
}

// TODO: Should be used with references instead of pointers.
template<typename T> PBR_CNSTEXPR PBR_INLINE
bool Bounds3<T>::IntersectP(const Ray_arg ray, fp_t *out_hit0 /*=nullptr*/, fp_t *out_hit1 /*=nullptr*/) const
{
    fp_t t0 = 0, t1 = ray.tMax;

    for (int i = 0; i < 3; ++i) {
        // Update interval for i_th bounding box slab
        const fp_t invRayDir = 1 / ray.direction[i];
        fp_t tNear = (pMin[i] - ray.origin[i]) * invRayDir;
        fp_t tFar = (pMax[i] - ray.origin[i]) * invRayDir;

        if(tNear > tFar) std::swap(tNear, tFar);

// NOTE: Chapter 3.9.2
#if PBR_ENABLE_EFLOAT == 1
        tFar *= 1 + 2 * Gamma(3);
#endif

        if(tNear > t0) t0 = tNear;
        if(tFar < t1) t1 = tFar;

        if(t0 > t1)
            return false;
    }

    if (out_hit0 != nullptr) *out_hit0 = t0;
    if (out_hit1 != nullptr) *out_hit1 = t1;

    return true;
}

// NOTE: WTF is this style, what is this dirIsNeg
// NOTE: Book claims 15% performance improvement with this method over it's overloaded brother.
template<typename T> PBR_CNSTEXPR PBR_INLINE
bool Bounds3<T>::IntersectP(const Ray_arg ray, const Vector3_arg<T> invDir, const i32 dirIsNeg[3]) const
{
    // NOTE: Why in the book is bounds of type Bounds3f and not Bounds3<T>, may be there is no reason Bounds3 should be templated at all.
    const Bounds3<T> &bounds = *this;
    // Check for ray intersection against 'x' and 'y' slabs
    fp_t tMin = (bounds[dirIsNeg[0]].x - ray.origin.x) * invDir.x;
    fp_t tMax = (bounds[1 - dirIsNeg[0]].x - ray.origin.x) * invDir.x;

    fp_t tyMin = (bounds[dirIsNeg[1]].y - ray.origin.y) * invDir.y;
    fp_t tyMax = (bounds[1 - dirIsNeg[1]].y - ray.origin.y) * invDir.y;

// NOTE: Chapter 3.9.2
#if PBR_ENABLE_EFLOAT == 1
    tMax *= 1 + 2 * Gamma(3);
    tyMax *= 1 + 2 * Gamma(3);
#endif

    if(tMin > tyMax || tyMin > tMax)
        return false;
    if(tyMin > tMin) tMin = tyMin;
    if(tyMax < tMax) tMax = tyMax;

    fp_t tzMin = (bounds[dirIsNeg[2]].z - ray.origin.z) * invDir.z;
    fp_t tzMax = (bounds[1 - dirIsNeg[2]].z - ray.origin.z) * invDir.z;
#if PBR_ENABLE_EFLOAT == 1
    tzMax *= 1 + 2 * Gamma(3);
#endif

    if(tMin > tzMax || tzMin > tMax)
        return false;
    if(tzMin > tMin) tMin = tzMin;
    if(tzMax < tMax) tMax = tzMax;

    return (tMin < ray.tMax) && (tMax > 0);
}


// ---------------------------------------
// ---------- UTILITY FUNCTIONS ----------
// ---------------------------------------

// NOTE: Next 3 functions assign pMin and pMax directly instead of using constructor,
//       because it's shit, but i don't know what can i do with it. And as stated in pbrt implementation
//       this in turn, breaks returning an invalid bound for the case where we Intersect() non-overlapping bounds.
template<typename T> PBR_CNSTEXPR PBR_INLINE
Bounds3<T> Union(const Bounds3_arg<T> b, const Point3_arg<T> p)
{
    Bounds3<T> result;
    result.pMin = Min(b.pMin, p);
    result.pMax = Max(b.pMax, p)
    return result;
}

template<typename T> PBR_CNSTEXPR PBR_INLINE
Bounds3<T> Union(const Bounds3_arg<T> b1, const Bounds3_arg<T> b2)
{
    Bounds3<T> result;
    result.pMin = Min(b1.pMin, b2.pMin);
    result.pMax = Max(b1.pMax, b2.pMax)
    return result;
}

template<typename T> PBR_CNSTEXPR PBR_INLINE
Bounds3<T> Intersect(const Bounds3_arg<T> b1, const Bounds3_arg<T> b2)
{
    Bounds3<T> result;
    result.pMin = Max(b1.pMin, b2.pMin);
    result.pMax = Min(b1.pMax, b2.pMax)
    return result;
}

// TODO: define compare operator for two Point3 ?
template<typename T> PBR_CNSTEXPR PBR_INLINE
bool Overlaps(const Bounds3_arg<T> b1, const Bounds3_arg<T> b2)
{
    // NOTE: not sure how it works monkaHmm
    bool x = (b1.pMax.x >= b2.pMin.x) && (b1.pMin.x <= b2.pMax.x);
    bool y = (b1.pMax.y >= b2.pMin.y) && (b1.pMin.y <= b2.pMax.y);
    bool z = (b1.pMax.z >= b2.pMin.z) && (b1.pMin.z <= b2.pMax.z);
    return (x && y && z);
}

// Determine if a given point is inside the bounding box.
template<typename T> PBR_CNSTEXPR PBR_INLINE
bool Inside(const Point3_arg<T> p, const Bounds3_arg<T> b)
{
    return (p.x >= b.pMin.x && p.x <= b.pMax.x &&
            p.y >= b.pMin.y && p.y <= b.pMax.y &&
            p.z >= b.pMin.z && p.z <= b.pMax.z);
}

// TODO: define compare operator for two Point3 ?
// Determine if a given point is inside the bounding box, doesn't consider points on
// the upper boundary to be inside the bounds.
template<typename T> PBR_CNSTEXPR PBR_INLINE
bool InsideExclusive(const Point3_arg<T> p, const Bounds3_arg<T> b)
{
    return (p.x >= b.pMin.x && p.x < b.pMax.x &&
            p.y >= b.pMin.y && p.y < b.pMax.y &&
            p.z >= b.pMin.z && p.z < b.pMax.z);
}


#pragma endregion Bounds3

PBR_NAMESPACE_END

#undef PBR_CNSTEXPR
#undef PBR_INLINE
