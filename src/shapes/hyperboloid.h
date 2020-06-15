#pragma once

#include "../core/shape.h"


PBR_NAMESPACE_BEGIN

// NOTE: Nice variables naming, guys from the book.
// class Hyperboloid : public Shape
// {
// public:
//     Hyperboloid(const Transform *ObjectToWorld, const Transform *WorldToObject, bool reverseOrientation,
//                 Point3_arg<fp_t> point1, Point3_arg<fp_t> point2, fp_t tm);


//     Bounds3_t ObjectBound() const override;

//     // NOTE: testAlphaTexture is not used
//     bool Intersect(const Ray_arg r,
//                    fp_t &out_tHit, SurfaceInteraction &out_isect,
//                    bool /*testAlphaTexture = true*/) const override;
//     // NOTE: testAlphaTexture is not used
//     bool IsIntersecting(const Ray_arg r, bool /*testAlphaTexture = true*/) const override;

//     fp_t Area() const override;


// private:
//     const Point3_t m_point1, m_point2;
//     const fp_t m_zMin, m_zMax;
//     const fp_t m_phiMax;
//     fp_t m_rMax;
//     fp_t m_ah, m_ch;
// };

PBR_NAMESPACE_END
