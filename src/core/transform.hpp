#pragma once

#include "core.hpp"
#include "pbr_math.hpp"

#include "geometry.hpp"

#define PBR_CNSTEXPR constexpr
#define PBR_INLINE inline


namespace pbr {

// ******************************************************************************
// --------------------------------- Matrix4x4 ----------------------------------
// ******************************************************************************

#pragma region Matrix4x4

// Matrix4x4, row major
struct Matrix4x4
{
    // Creates Identity matrix
    PBR_CNSTEXPR Matrix4x4();
    PBR_CNSTEXPR explicit Matrix4x4(const fp_t matrix[4][4]);
    PBR_CNSTEXPR explicit Matrix4x4(fp_t t00, fp_t t01, fp_t t02, fp_t t03,
                                    fp_t t10, fp_t t11, fp_t t12, fp_t t13,
                                    fp_t t20, fp_t t21, fp_t t22, fp_t t23,
                                    fp_t t30, fp_t t31, fp_t t32, fp_t t33);

    PBR_CNSTEXPR PBR_INLINE fp_t* operator[](const int i32);
    PBR_CNSTEXPR PBR_INLINE const fp_t* operator[](const int i32) const;


    fp_t m[4][4];
};

// NOTE: I don't think there is a point of doing that, for an object of such a size
using Matrix4x4_arg = Matrix4x4&;


// ---------------------------------------
// ------------ CONSTRUCTORS -------------
// ---------------------------------------

// NOTE: why did I do this, no one will ever know,
// but actually it's because I cannot use C11 array initialization in C++,
// no blame on me
PBR_CNSTEXPR
Matrix4x4::Matrix4x4()
    : m { static_cast<fp_t>(1), static_cast<fp_t>(0), static_cast<fp_t>(0), static_cast<fp_t>(0),
          static_cast<fp_t>(0), static_cast<fp_t>(1), static_cast<fp_t>(0), static_cast<fp_t>(0),
          static_cast<fp_t>(0), static_cast<fp_t>(0), static_cast<fp_t>(1), static_cast<fp_t>(0),
          static_cast<fp_t>(0), static_cast<fp_t>(0), static_cast<fp_t>(0), static_cast<fp_t>(1) }
{}

PBR_CNSTEXPR
Matrix4x4::Matrix4x4(const fp_t matrix[4][4])
    : m { matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
          matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
          matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3],
          matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3] }
{}

PBR_CNSTEXPR
Matrix4x4::Matrix4x4(fp_t t00, fp_t t01, fp_t t02, fp_t t03,
                     fp_t t10, fp_t t11, fp_t t12, fp_t t13,
                     fp_t t20, fp_t t21, fp_t t22, fp_t t23,
                     fp_t t30, fp_t t31, fp_t t32, fp_t t33)
    : m { t00, t01, t02, t03,
          t10, t11, t12, t13,
          t20, t21, t22, t23,
          t30, t31, t32, t33 }
{}


// ---------------------------------------
// ----------- ACCESS OPERATOR ------------
// ---------------------------------------

PBR_CNSTEXPR PBR_INLINE
fp_t* Matrix4x4::operator[](const i32 i)
{
    PBR_ASSERT(i >= 0 && i <= 15);
    return this->m[i];
}

PBR_CNSTEXPR PBR_INLINE
const fp_t* Matrix4x4::operator[](const i32 i) const
{
    PBR_ASSERT(i >= 0 && i <= 15);
    return this->m[i];
}


// ---------------------------------------
// ---------- UTILITY FUNCTIONS ----------
// ---------------------------------------

PBR_CNSTEXPR PBR_INLINE
Matrix4x4 Transpose(const Matrix4x4_arg m)
{
    return Matrix4x4(m[0][0], m[1][0], m[2][0], m[3][0],
                     m[0][1], m[1][1], m[2][1], m[3][1],
                     m[0][2], m[1][2], m[2][2], m[3][2],
                     m[0][3], m[1][3], m[2][3], m[3][3]);
}

// TODO: loop unroll ?
PBR_CNSTEXPR PBR_INLINE
Matrix4x4 Mul(const Matrix4x4_arg m1, const Matrix4x4_arg m2)
{
    // creates identity matrix so we need to reset the diagonal
    Matrix4x4 result;
    result[0][0] = result[1][1] = result[2][2]= result[3][3] = static_cast<fp_t>(0);

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 4; k++)
                result[i][j] += m1[i][k] * m2[k][j];

    return result;
}

// TODO: Definitely needs to be revisited, as I understand it's not numerically stable
// taken from https://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
// may be take a look at https://stackoverflow.com/a/29686652
PBR_CNSTEXPR
Matrix4x4 Inverse(const Matrix4x4_arg m) 
{
    fp_t A2323 = m[2][2] * m[3][3] - m[2][3] * m[3][2];
    fp_t A1323 = m[2][1] * m[3][3] - m[2][3] * m[3][1];
    fp_t A1223 = m[2][1] * m[3][2] - m[2][2] * m[3][1];
    fp_t A0323 = m[2][0] * m[3][3] - m[2][3] * m[3][0];
    fp_t A0223 = m[2][0] * m[3][2] - m[2][2] * m[3][0];
    fp_t A0123 = m[2][0] * m[3][1] - m[2][1] * m[3][0];
    fp_t A2313 = m[1][2] * m[3][3] - m[1][3] * m[3][2];
    fp_t A1313 = m[1][1] * m[3][3] - m[1][3] * m[3][1];
    fp_t A1213 = m[1][1] * m[3][2] - m[1][2] * m[3][1];
    fp_t A2312 = m[1][2] * m[2][3] - m[1][3] * m[2][2];
    fp_t A1312 = m[1][1] * m[2][3] - m[1][3] * m[2][1];
    fp_t A1212 = m[1][1] * m[2][2] - m[1][2] * m[2][1];
    fp_t A0313 = m[1][0] * m[3][3] - m[1][3] * m[3][0];
    fp_t A0213 = m[1][0] * m[3][2] - m[1][2] * m[3][0];
    fp_t A0312 = m[1][0] * m[2][3] - m[1][3] * m[2][0];
    fp_t A0212 = m[1][0] * m[2][2] - m[1][2] * m[2][0];
    fp_t A0113 = m[1][0] * m[3][1] - m[1][1] * m[3][0];
    fp_t A0112 = m[1][0] * m[2][1] - m[1][1] * m[2][0];

    fp_t det = m[0][0] * ( m[1][1] * A2323 - m[1][2] * A1323 + m[1][3] * A1223 ) 
             - m[0][1] * ( m[1][0] * A2323 - m[1][2] * A0323 + m[1][3] * A0223 ) 
             + m[0][2] * ( m[1][0] * A1323 - m[1][1] * A0323 + m[1][3] * A0123 ) 
             - m[0][3] * ( m[1][0] * A1223 - m[1][1] * A0223 + m[1][2] * A0123 );
    det = 1 / det;

    return Matrix4x4(det *   ( m[1][1] * A2323 - m[1][2] * A1323 + m[1][3] * A1223 ),
                     det * - ( m[0][1] * A2323 - m[0][2] * A1323 + m[0][3] * A1223 ),
                     det *   ( m[0][1] * A2313 - m[0][2] * A1313 + m[0][3] * A1213 ),
                     det * - ( m[0][1] * A2312 - m[0][2] * A1312 + m[0][3] * A1212 ),
                     det * - ( m[1][0] * A2323 - m[1][2] * A0323 + m[1][3] * A0223 ),
                     det *   ( m[0][0] * A2323 - m[0][2] * A0323 + m[0][3] * A0223 ),
                     det * - ( m[0][0] * A2313 - m[0][2] * A0313 + m[0][3] * A0213 ),
                     det *   ( m[0][0] * A2312 - m[0][2] * A0312 + m[0][3] * A0212 ),
                     det *   ( m[1][0] * A1323 - m[1][1] * A0323 + m[1][3] * A0123 ),
                     det * - ( m[0][0] * A1323 - m[0][1] * A0323 + m[0][3] * A0123 ),
                     det *   ( m[0][0] * A1313 - m[0][1] * A0313 + m[0][3] * A0113 ),
                     det * - ( m[0][0] * A1312 - m[0][1] * A0312 + m[0][3] * A0112 ),
                     det * - ( m[1][0] * A1223 - m[1][1] * A0223 + m[1][2] * A0123 ),
                     det *   ( m[0][0] * A1223 - m[0][1] * A0223 + m[0][2] * A0123 ),
                     det * - ( m[0][0] * A1213 - m[0][1] * A0213 + m[0][2] * A0113 ),
                     det *   ( m[0][0] * A1212 - m[0][1] * A0212 + m[0][2] * A0112 ));
}

#pragma endregion Matrix4x4


// ******************************************************************************
// --------------------------------- Transform ----------------------------------
// ******************************************************************************

#pragma region Transform

// TODO: AnimatedTransform and Quaternion not implemented
// TODO: Comparison operator and IsDentity method not implemented
// TODO: I don't understand HasScale() method, so I won't implement it for now
// TODO: I think Transformations should be more like in GLM, so matrices can be reused,
//       without creating new matrix for every operation
struct Transform
{
    // NOTE: there is an empty constructor in the original implementation
    PBR_CNSTEXPR explicit Transform(const fp_t matrix[4][4]);
    PBR_CNSTEXPR explicit Transform(const Matrix4x4_arg matrix);
    PBR_CNSTEXPR explicit Transform(const Matrix4x4_arg matrix, const Matrix4x4_arg inverse);

    PBR_CNSTEXPR bool SwapsHandedness() const;

    // TODO: operator() is templated in the original for some reason that needs to be figured out
    PBR_CNSTEXPR PBR_INLINE Point3_t operator()(const Point3_arg<fp_t> p) const;
    PBR_CNSTEXPR PBR_INLINE Vector3_t operator()(const Vector3_arg<fp_t> v) const;
    PBR_CNSTEXPR PBR_INLINE Normal3_t operator()(const Normal3_arg<fp_t> n) const;
    //PBR_CNSTEXPR PBR_INLINE Ray operator()(const Ray_arg r) const;
    //PBR_CNSTEXPR PBR_INLINE RayDifferential operator()(const RayDifferential_arg r) const;
    //PBR_CNSTEXPR PBR_INLINE Bounds3_t operator()(const Bounds3_arg<fp_t> b) const;

// NOTE: marked as private in the original implementation
    Matrix4x4 m;
    Matrix4x4 mInv;
};


// ---------------------------------------
// ------------ CONSTRUCTORS -------------
// ---------------------------------------

PBR_CNSTEXPR
Transform::Transform(const fp_t matrix[4][4])
    : m(matrix)
    , mInv(Inverse(m))
{}

PBR_CNSTEXPR
Transform::Transform(const Matrix4x4_arg matrix)
    : m(matrix)
    , mInv(Inverse(m))
{}

PBR_CNSTEXPR
Transform::Transform(const Matrix4x4_arg matrix, const Matrix4x4_arg inverse)
    : m(matrix)
    , mInv(inverse)
{}


// ---------------------------------------
// ----- BINARY ARITHMETIC OPERATORS -----
// ---------------------------------------

// TODO: Why overload operator* for transform and not for Mtrix4x4
PBR_CNSTEXPR
Transform operator*(const Transform &t1, const Transform &t2) {
    return Transform(Mul(t1.m, t2.m),
                     Mul(t2.mInv, t1.mInv));
}


// ---------------------------------------
// ------- FUNCTION CALL OPERATORS -------
// ---------------------------------------

// TODO: As stated int the book most of the time wp will be equal to 1,
//       only the projective transformation will require division
// NOTE: I think m[3] row will be zero every time and only m[3][3] will be non zero,
//       so it can can be optimized
PBR_CNSTEXPR PBR_INLINE
Point3_t Transform::operator()(const Point3_arg<fp_t> p) const {
    const fp_t xp = m[0][0] * p.x + m[0][1] * p.y + m[0][2] * p.z + m[0][3];
    const fp_t yp = m[1][0] * p.x + m[1][1] * p.y + m[1][2] * p.z + m[1][3];
    const fp_t zp = m[2][0] * p.x + m[2][1] * p.y + m[2][2] * p.z + m[2][3];
    const fp_t wp = m[3][0] * p.x + m[3][1] * p.y + m[3][2] * p.z + m[3][3];
    PBR_ASSERT(wp != 0);
    if (wp == 1)
        return Point3_t(xp, yp, zp);
    else
        // NOTE: Will be Point3_t(xp / wp, yp / wp, zp / wp); more effective ?
        return Point3_t(xp, yp, zp) / wp;
}

PBR_CNSTEXPR PBR_INLINE
Vector3_t Transform::operator()(const Vector3_arg<fp_t> v) const
{
    return Vector3_t(m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
                     m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
                     m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z);
}

PBR_CNSTEXPR PBR_INLINE
Normal3_t Transform::operator()(const Normal3_arg<fp_t> n) const
{
    return Normal3_t(mInv[0][0] * n.x + mInv[1][0] * n.y + mInv[2][0] * n.z,
                     mInv[0][1] * n.x + mInv[1][1] * n.y + mInv[2][1] * n.z,
                     mInv[0][2] * n.x + mInv[1][2] * n.y + mInv[2][2] * n.z);
}

// TODO: I don't understand shit about implementation of this method
// PBR_CNSTEXPR PBR_INLINE
// Ray Transform::operator()(const Ray_arg r) const
// {
//     return ;
// }

// TODO: same as for Ray
// PBR_CNSTEXPR PBR_INLINE
// RayDifferential Transform::operator()(const RayDifferential_arg r) const
// {
//     return ;
// }

// TODO: same as for Ray, and definitely should be optimized
// PBR_CNSTEXPR PBR_INLINE
// Bounds3_t Transform::operator()(const Bounds3_arg<fp_t> &b) const {
//     const Transform &M = *this;
//     Bounds3_t ret(M(Point3f(b.pMin.x, b.pMin.y, b.pMin.z)));
//     ret = Union(ret, M(Point3f(b.pMax.x, b.pMin.y, b.pMin.z)));
//     ret = Union(ret, M(Point3f(b.pMin.x, b.pMax.y, b.pMin.z)));
//     ret = Union(ret, M(Point3f(b.pMin.x, b.pMin.y, b.pMax.z)));
//     ret = Union(ret, M(Point3f(b.pMin.x, b.pMax.y, b.pMax.z)));
//     ret = Union(ret, M(Point3f(b.pMax.x, b.pMax.y, b.pMin.z)));
//     ret = Union(ret, M(Point3f(b.pMax.x, b.pMin.y, b.pMax.z)));
//     ret = Union(ret, M(Point3f(b.pMax.x, b.pMax.y, b.pMax.z)));
//     return ret;
// }


// ---------------------------------------
// ---------- UTILITY FUNCTIONS ----------
// ---------------------------------------

// NOTE: Method name is shit
PBR_CNSTEXPR
bool Transform::SwapsHandedness() const
{
    fp_t det = m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
               m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
               m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
    return det < 0;
}


PBR_CNSTEXPR
Transform Inverse(const Transform &t)
{
    return Transform(t.mInv, t.m);
}

PBR_CNSTEXPR
Transform Transpose(const Transform &t)
{
    return Transform(Transpose(t.m), Transpose(t.mInv));
}

PBR_CNSTEXPR
Transform Translate(const Vector3_arg<fp_t> delta)
{
    Matrix4x4 m(1, 0, 0, delta.x,
                0, 1, 0, delta.y,
                0, 0, 1, delta.z,
                0, 0, 0, 1);
    Matrix4x4 mInv(1, 0, 0, -delta.x,
                   0, 1, 0, -delta.y,
                   0, 0, 1, -delta.z,
                   0, 0, 0, 1);
    return Transform(m, mInv);
}

// NOTE: why in the book Translate argument is Vector3, 
//       and Scale arguments is 3 fp_t ?
PBR_CNSTEXPR
Transform Scale(const fp_t x, const fp_t y, const fp_t z)
{
    Matrix4x4 m(x, 0, 0, 0,
                0, y, 0, 0,
                0, 0, z, 0,
                0, 0, 0, 1);
    Matrix4x4 mInv(1/x,   0,   0, 0,
                     0, 1/y,   0, 0,
                     0,   0, 1/z, 0,
                     0,   0,   0, 1);
    return Transform(m, mInv);
}

Transform RotateX(const fp_t theta)
{
    const fp_t sinTheta = pbr::Sin(Radians(theta));
    const fp_t cosTheta = pbr::Cos(Radians(theta));
    Matrix4x4 m(1,        0,         0, 0,
                0, cosTheta, -sinTheta, 0,
                0, sinTheta,  cosTheta, 0,
                0,        0,         0, 1);
    return Transform(m, Transpose(m));
}

Transform RotateY(const fp_t theta)
{
    const fp_t sinTheta = pbr::Sin(Radians(theta));
    const fp_t cosTheta = pbr::Cos(Radians(theta));
    Matrix4x4 m( cosTheta, 0, sinTheta, 0,
                        0, 1,        0, 0,
                -sinTheta, 0, cosTheta, 0,
                        0, 0,        0, 1);
    return Transform(m, Transpose(m));
}

Transform RotateZ(const fp_t theta)
{
    const fp_t sinTheta = pbr::Sin(Radians(theta));
    const fp_t cosTheta = pbr::Cos(Radians(theta));
    Matrix4x4 m(cosTheta, -sinTheta, 0, 0,
                sinTheta,  cosTheta, 0, 0,
                       0,         1, 1, 0,
                       0,         0, 0, 1);
    return Transform(m, Transpose(m));
}

// TODO: implementation slightly differs from original, needs to be checked
//       may be there is a precision loss
Transform Rotate(const fp_t theta, const Vector3_arg<fp_t> axis) {
    const auto a = Normalize(axis);
    const fp_t sinTheta = pbr::Sin(Radians(theta));
    const fp_t cosTheta = pbr::Cos(Radians(theta));

    // Compute rotation of first basis vector
    const fp_t m00 = a.x * a.x * (1 - cosTheta) + cosTheta;
    const fp_t m01 = a.x * a.y * (1 - cosTheta) - a.z * sinTheta;
    const fp_t m02 = a.x * a.z * (1 - cosTheta) + a.y * sinTheta;
    // Compute rotations of second basis vector
    const fp_t m10 = a.x * a.y * (1 - cosTheta) + a.z * sinTheta;
    const fp_t m11 = a.y * a.y + (1 - a.y * a.y) * cosTheta;
    const fp_t m12 = a.y * a.z * (1 - cosTheta) - a.x * sinTheta;
    // Compute rotations of third basis vector
    const fp_t m20 = a.x * a.z * (1 - cosTheta) - a.y * sinTheta;
    const fp_t m21 = a.y * a.z * (1 - cosTheta) + a.x * sinTheta;
    const fp_t m22 = a.z * a.z * (1 - cosTheta) + cosTheta;

    Matrix4x4 m(m00, m01, m02, 0,
                m10, m11, m12, 0,
                m20, m21, m22, 0,
                  0,   0,   0, 0);
    return Transform(m, Transpose(m));
}

// TODO: matrix inverse can be simplified in this case
Transform LookAt(const Point3_arg<fp_t> pos, const Point3_arg<fp_t> look, const Vector3_arg<fp_t> up) {
    const auto direction = Normalize(look - pos);
    const auto right = Normalize(Cross(Normalize(up), direction));
    const auto newUp = Cross(direction, right);

    PBR_ASSERT_MSG(Cross(Normalize(up), direction).Length() == 0,
        "up vector and viewing direction passed to LookAt are pointing in the same direction");

    Matrix4x4 cameraToWorld(right.x, newUp.x, direction.x, pox.x,
                            right.y, newUp.y, direction.y, pos.y,
                            right.z, newUp.z, direction.z, pos.z,
                                  0,       0,           0,     1);
    return Transform(Inverse(cameraToWorld), cameraToWorld);
}

#pragma endregion Transform

} // namespace pbr
