#pragma once

#include "../core/shape.h"


namespace pbr {

class Sphere : public Shape
{
public:
    Sphere(const Transform *ObjectToWorld, const Transform *WorldToObject, bool reverseOrientation,
           fp_t radius, fp_t zMin, fp_t zMax, fp_t phiMax);


    Bounds3_t ObjectBound() const override;

    bool Intersect(const Ray_arg ray,
                   fp_t *out_tHit, SurfaceInteraction *out_isect,
                   bool testAlphaTexture = true) const override;
    bool IntersectP(const Ray_arg ray, bool testAlphaTexture = true) const override;

    fp_t Area() const override;


private:
    const fp_t m_radius;
    const fp_t m_zMin, m_zMax;
    const fp_t m_thetaMin, m_thetaMax;
    const fp_t m_phiMax;
};


// ---------------------------------------
// ------------ CONSTRUCTORS -------------
// ---------------------------------------

// FINDOUT: Will compiler optimize this repetitive code ?
// NOTE: And why the fuck there is a need in min(), max() at all?
//       And what zMin/zMax will be used in thetaMin/Max initialization (since I changed naming of private fields there is no problem, but now it may work differently) ?
//       What's the use of std::clamp for phiMax, it should be error if phiMax < 0 or > 360, not clamp. Probably same with zMin/zMax, they shouldn't be more than radius.
Sphere::Sphere(const Transform *ObjectToWorld, const Transform *WorldToObject, bool reverseOrientation,
               fp_t radius, fp_t zMin, fp_t zMax, fp_t phiMax)
    : Shape(ObjectToWorld, WorldToObject, reverseOrientation)
    , m_radius(radius)
    , m_zMin(std::clamp(std::min(zMin, zMax), -radius, radius))
    , m_zMax(std::clamp(std::max(zMin, zMax), -radius, radius))
    , m_thetaMin(pbr::ACos(std::clamp(zMin / radius, fp_t(-1), fp_t(1))))
    , m_thetaMax(pbr::ACos(std::clamp(zMax / radius, fp_t(-1), fp_t(1))))
    , m_phiMax(pbr::Radians(std::clamp(phiMax, fp_t(0), fp_t(360))))
{}


// ---------------------------------------
// --------------- METHODS ---------------
// ---------------------------------------

// TODO: Can be improved as stated in 3.2.1 chapter
Bounds3_t Sphere::ObjectBound() const
{
    return Bounds3_t(Point3_t(-m_radius, -m_radius, m_zMin),
                     Point3_t( m_radius,  m_radius, m_zMax));
}

bool Sphere::Intersect(const Ray_arg ray,
                       fp_t *out_tHit, SurfaceInteraction *out_isect,
                       bool testAlphaTexture = true) const
{


}


} // namespace pbr
