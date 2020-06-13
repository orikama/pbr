#include "cylinder.h"
#include "../core/stats.h"
#include "../core/efloat.hpp"


PBR_NAMESPACE_BEGIN

// ---------------------------------------
// ------------ CONSTRUCTORS -------------
// ---------------------------------------

// FINDOUT: Will compiler optimize this repetitive code ?
// NOTE: And why the fuck there is a need in min(), max() at all?
Cylinder::Cylinder(const Transform *ObjectToWorld, const Transform *WorldToObject, bool reverseOrientation,
                   fp_t radius, fp_t zMin, fp_t zMax, fp_t phiMax)
    : Shape(ObjectToWorld, WorldToObject, reverseOrientation)
    , m_radius(radius)
    , m_zMin(std::min(zMin, zMax))
    , m_zMax(std::max(zMin, zMax))
    , m_phiMax(pbr::Radians(std::clamp(phiMax, fp_t(0), fp_t(360))))
{}


// ---------------------------------------
// --------------- METHODS ---------------
// ---------------------------------------

// TODO: Can be improved as stated in 3.3.1 chapter
Bounds3_t Cylinder::ObjectBound() const
{
    return Bounds3_t(Point3_t(-m_radius, -m_radius, m_zMin),
                     Point3_t( m_radius,  m_radius, m_zMax));
}

// NOTE: testAlphaTexture is not used
bool Cylinder::Intersect(const Ray_arg r,
                         fp_t &out_tHit, SurfaceInteraction &out_isect,
                         bool /*testAlphaTexture = true*/) const
{
    PBR_PROFILE_FUNCTION(ProfileCategory::Shape_Intersect)

    // Transform ray to object space
    Vector3_t oError, dError;
    Ray ray = (*WorldToObject)(r, oError, dError);

    // Initialize EFloat ray coordinate values;
    EFloat ox(ray.origin.x, oError.x), oy(ray.origin.y, oError.y);
    EFloat dx(ray.direction.x, dError.x), dy(ray.direction.y, dError.y);
    // Compute quadratic cylinder coefficients
    EFloat a = dx * dx + dy * dy;
    EFloat b = 2 * (dx * ox + dy * oy);
    EFloat c = ox * ox + oy * oy - EFloat(m_radius) * EFloat(m_radius);
    // Solve quadratic equation
    EFloat t0, t1;
    if (Quadratic(a, b, c, t0, t1) == false)
        return false;
    // After Quadratic() t0 will be <= t1, so only need to check this 2 conditions
    if (t0.UpperBound() > ray.tMax || t1.LowerBound() <= 0)
        return false;
    EFloat tCylinderHit = t0;
    if (tCylinderHit.LowerBound() <= 0) {
        tCylinderHit = t1;
        if (tCylinderHit.UpperBound() > ray.tMax)
            return false;
    }
    // Compute cylinder hit(intersection) point and phi
    Point3_t pHit = ray((fp_t)tCylinderHit);
//#if PBR_PBR_ENABLE_EFLOAT == 1
    // Refine cylinder intersection point
    fp_t hitRadius = pbr::Sqrt(pHit.x * pHit.x + pHit.y + pHit.y);
    pHit.x *= m_radius / hitRadius;
    pHit.y *= m_radius / hitRadius;
//#endif
    fp_t phi = pbr::ATan2(pHit.y, pHit.x);
    if (phi < 0) phi += constants::pi_t * 2;

    // TODO: This shit is repetitive and definitely can be optimized
    // Test cylinder intersection against clipping parametrs
    if (pHit.z < m_zMin || pHit.z > m_zMax || phi > m_phiMax) {
        if (tCylinderHit == t1 && t1.UpperBound() > ray.tMax)
            return false;
        tCylinderHit = t1;
        // Compute sphere hit(intersection) point and phi
        pHit = ray((fp_t)tCylinderHit);
        // Refine cylinder intersection point
        fp_t hitRadius = pbr::Sqrt(pHit.x * pHit.x + pHit.y + pHit.y);
        pHit.x *= m_radius / hitRadius;
        pHit.y *= m_radius / hitRadius;
        fp_t phi = pbr::ATan2(pHit.y, pHit.x);
        if (phi < 0) phi += constants::pi_t * 2;
        if (pHit.z < m_zMin || pHit.z > m_zMax || phi > m_phiMax)
            return false;
    }

    // Find parametric representation of cylinder hit
    fp_t u = phi / m_phiMax;
    fp_t v = (pHit.z - m_zMin) / (m_zMax - m_zMin);
    // Compute cylinder dpdu and dpdv
    Vector3_t dpdu(-m_phiMax * pHit.y, m_phiMax * pHit.x, 0);
    Vector3_t dpdv(0, 0, m_zMax - m_zMin);

    // NOTE: This stuff(Partial derivatives of Noprmal Vectors, Weingarten equations) I don't understand.
    // Compute second derivatives
    Vector3_t d2pduu = -m_phiMax * m_phiMax * Vector3_t(pHit.x, pHit.y, 0);
    Vector3_t d2pduv(0);
    Vector3_t d2pdvv(0);
    // Compute coefficients for fundamental forms
    // TODO: Since d2pduv and d2pdvv = 0 and dpdv almost = 0, this whole thing can be simplified.
    fp_t E = Dot(dpdu, dpdu);
    fp_t F = Dot(dpdu, dpdv);
    fp_t G = Dot(dpdv, dpdv);
    Vector3_t N = Normalize(Cross(dpdu, dpdv)); // NOTE: This vector will be computed again in SurfaceInteraction constructor.
    fp_t e = Dot(N, d2pduu);
    fp_t f = Dot(N, d2pduv);
    fp_t g = Dot(N, d2pdvv);
    // Compute Partial derivatives of Noprmal Vectors from fundamental form coefficients
    fp_t invEGF2 = fp_t(1) / (E * G - F * F);
    Normal3_t dndu((f * F - e * G) * invEGF2 * dpdu + (e * F - f * E) * invEGF2 * dpdv);
    Normal3_t dndv((g * F - f * G) * invEGF2 * dpdu + (f * F - g * E) * invEGF2 * dpdv);

    // NOTE: This thing is nessesary for SurfaceConstruction anyway.
//#if PBR_PBR_ENABLE_EFLOAT == 1
    // Compute error bounds for sphere intersection
    Vector3_t pError(Gamma(3) * pbr::Abs(Vector3_t(pHit.x, pHit.y, 0)));
//#endif

    out_tHit = fp_t(tCylinderHit);
    out_isect = (*ObjectToWorld)(SurfaceInteraction(pHit, pError,
                                                    Point2_t(u, v), -ray.direction,
                                                    dpdu, dpdv, dndu, dndv,
                                                    ray.time, this));

    return true;
}

// NOTE: testAlphaTexture is not used
bool Cylinder::IsIntersecting(const Ray_arg r, bool /*testAlphaTexture = true*/) const
{
    PBR_PROFILE_FUNCTION(ProfileCategory::Shape_IsIntersecting)

    // Transform ray to object space
    Vector3_t oError, dError;
    Ray ray = (*WorldToObject)(r, oError, dError);

    // Initialize EFloat ray coordinate values;
    EFloat ox(ray.origin.x, oError.x), oy(ray.origin.y, oError.y);
    EFloat dx(ray.direction.x, dError.x), dy(ray.direction.y, dError.y);
    // Compute quadratic cylinder coefficients
    EFloat a = dx * dx + dy * dy;
    EFloat b = 2 * (dx * ox + dy * oy);
    EFloat c = ox * ox + oy * oy - EFloat(m_radius) * EFloat(m_radius);
    // Solve quadratic equation
    EFloat t0, t1;
    if (Quadratic(a, b, c, t0, t1) == false)
        return false;
    // After Quadratic() t0 will be <= t1, so only need to check this 2 conditions
    if (t0.UpperBound() > ray.tMax || t1.LowerBound() <= 0)
        return false;
    EFloat tCylinderHit = t0;
    if (tCylinderHit.LowerBound() <= 0) {
        tCylinderHit = t1;
        if (tCylinderHit.UpperBound() > ray.tMax)
            return false;
    }
    // Compute cylinder hit(intersection) point and phi
    Point3_t pHit = ray((fp_t)tCylinderHit);
//#if PBR_PBR_ENABLE_EFLOAT == 1
    // Refine cylinder intersection point
    fp_t hitRadius = pbr::Sqrt(pHit.x * pHit.x + pHit.y + pHit.y);
    pHit.x *= m_radius / hitRadius;
    pHit.y *= m_radius / hitRadius;
//#endif
    fp_t phi = pbr::ATan2(pHit.y, pHit.x);
    if (phi < 0) phi += constants::pi_t * 2;

    // TODO: This shit is repetitive and definitely can be optimized
    // Test cylinder intersection against clipping parametrs
    if (pHit.z < m_zMin || pHit.z > m_zMax || phi > m_phiMax) {
        if (tCylinderHit == t1 && t1.UpperBound() > ray.tMax)
            return false;
        tCylinderHit = t1;
        // Compute sphere hit(intersection) point and phi
        pHit = ray((fp_t)tCylinderHit);
        // Refine cylinder intersection point
        fp_t hitRadius = pbr::Sqrt(pHit.x * pHit.x + pHit.y + pHit.y);
        pHit.x *= m_radius / hitRadius;
        pHit.y *= m_radius / hitRadius;
        fp_t phi = pbr::ATan2(pHit.y, pHit.x);
        if (phi < 0) phi += constants::pi_t * 2;
        if (pHit.z < m_zMin || pHit.z > m_zMax || phi > m_phiMax)
            return false;
    }

    return true;
}

fp_t Cylinder::Area() const
{
    return m_phiMax * m_radius * (m_zMax - m_zMin);
}

PBR_NAMESPACE_END
