#pragma once

#include "../core/shape.h"


PBR_NAMESPACE_BEGIN

class Sphere : public Shape
{
public:
    Sphere(const Transform *ObjectToWorld, const Transform *WorldToObject, bool reverseOrientation,
           fp_t radius, fp_t zMin, fp_t zMax, fp_t phiMax);


    Bounds3_t ObjectBound() const override;

    // NOTE: testAlphaTexture is not used
    bool Intersect(const Ray_arg r,
                   fp_t &out_tHit, SurfaceInteraction &out_isect,
                   bool /*testAlphaTexture = true*/) const override;
    // NOTE: testAlphaTexture is not used
    bool IsIntersecting(const Ray_arg r, bool /*testAlphaTexture = true*/) const override;

    fp_t Area() const override;


private:
    const fp_t m_radius;
    const fp_t m_zMin, m_zMax;
    const fp_t m_thetaMin, m_thetaMax;
    const fp_t m_phiMax;
};

PBR_NAMESPACE_END
