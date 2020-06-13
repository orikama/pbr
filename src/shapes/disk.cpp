#include "disk.h"
#include "../core/stats.h"
#include "../core/efloat.hpp"


PBR_NAMESPACE_BEGIN

// ---------------------------------------
// ------------ CONSTRUCTORS -------------
// ---------------------------------------

// FINDOUT: Get rid of std::clamp() ?
Disk::Disk(const Transform *ObjectToWorld, const Transform *WorldToObject, bool reverseOrientation,
           fp_t height, fp_t radius, fp_t innerRadius, fp_t phiMax)
    : Shape(ObjectToWorld, WorldToObject, reverseOrientation)
    , m_height(height)
    , m_radius(radius)
    , m_innerRadius(innerRadius)
    , m_phiMax(pbr::Radians(std::clamp(phiMax, fp_t(0), fp_t(360))))
{}


// ---------------------------------------
// --------------- METHODS ---------------
// ---------------------------------------

// TODO: Can be improved as stated in 3.3.1 chapter
Bounds3_t Disk::ObjectBound() const
{
    return Bounds3_t(Point3_t(-m_radius, -m_radius, m_height),
                     Point3_t( m_radius,  m_radius, m_height));
}

// NOTE: Don't understand how this whole thing with disk work.
// NOTE: testAlphaTexture is not used
bool Disk::Intersect(const Ray_arg r,
                     fp_t &out_tHit, SurfaceInteraction &out_isect,
                     bool /*testAlphaTexture = true*/) const
{
    PBR_PROFILE_FUNCTION(ProfileCategory::Shape_Intersect)

    // Transform ray to object space
    Vector3_t oError, dError;
    Ray ray = (*WorldToObject)(r, oError, dError);

    // Reject disk intersection for rays parallel to the disk's plane
    if (ray.direction.z == 0)
        return false;
    fp_t tDiskHit = (m_height - ray.origin.z) / ray.direction.z;
    if (tDiskHit <= 0 || tDiskHit >= ray.tMax)
        return false;

    // Compute cylinder hit(intersection) point and phi
    Point3_t pHit = ray(tDiskHit);
    fp_t dist2 = pHit.x * pHit.x + pHit.y * pHit.y;
    if (dist2 > m_radius * m_radius || dist2 < m_innerRadius * m_innerRadius)
        return false;
//#if PBR_PBR_ENABLE_EFLOAT == 1
    // Refine disk intersection point
    pHit.z = m_height;
//#endif
    fp_t phi = pbr::ATan2(pHit.y, pHit.x);
    if (phi < 0) phi += constants::pi_t * 2;
    if (phi > m_phiMax)
        return false;

    // Find parametric representation of disk hit
    fp_t u = phi / m_phiMax;
    fp_t dist2sqrt = pbr::Sqrt(dist2);
    fp_t v = (m_radius - dist2sqrt) / (m_radius - m_innerRadius);
    // Compute disk dpdu and dpdv
    Vector3_t dpdu(-m_phiMax * pHit.y, m_phiMax * pHit.x, 0);
    Vector3_t dpdv = ((m_innerRadius - m_radius) / dist2sqrt) * Vector3_t(pHit.x, pHit.y, 0);

    Normal3_t dndu(0);
    Normal3_t dndv(0);

    // NOTE: This thing is nessesary for SurfaceConstruction anyway.
//#if PBR_PBR_ENABLE_EFLOAT == 1
    // Compute error bounds for disk intersection
    Vector3_t pError(0);
//#endif

    out_tHit = fp_t(tDiskHit);
    out_isect = (*ObjectToWorld)(SurfaceInteraction(pHit, pError,
                                                    Point2_t(u, v), -ray.direction,
                                                    dpdu, dpdv, dndu, dndv,
                                                    ray.time, this));

    return true;
}

// NOTE: testAlphaTexture is not used
bool Disk::IsIntersecting(const Ray_arg r, bool /*testAlphaTexture = true*/) const
{
    PBR_PROFILE_FUNCTION(ProfileCategory::Shape_IsIntersecting)

    // Transform ray to object space
    Vector3_t oError, dError;
    Ray ray = (*WorldToObject)(r, oError, dError);

    // Reject disk intersection for rays parallel to the disk's plane
    if (ray.direction.z == 0)
        return false;
    fp_t tDiskHit = (m_height - ray.origin.z) / ray.direction.z;
    if (tDiskHit <= 0 || tDiskHit >= ray.tMax)
        return false;

    // Compute cylinder hit(intersection) point and phi
    Point3_t pHit = ray(tDiskHit);
    fp_t dist2 = pHit.x * pHit.x + pHit.y * pHit.y;
    if (dist2 > m_radius * m_radius || dist2 < m_innerRadius * m_innerRadius)
        return false;

    fp_t phi = pbr::ATan2(pHit.y, pHit.x);
    if (phi < 0) phi += constants::pi_t * 2;
    if (phi > m_phiMax)
        return false;

    return true;
}

fp_t Disk::Area() const
{
    return m_phiMax * fp_t(0.5) * (m_radius * m_radius - m_innerRadius * m_innerRadius);
}

PBR_NAMESPACE_END
