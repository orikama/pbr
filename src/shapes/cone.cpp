#include "cone.h"
#include "../core/stats.h"
#include "../core/efloat.hpp"


PBR_NAMESPACE_BEGIN

// ---------------------------------------
// ------------ CONSTRUCTORS -------------
// ---------------------------------------

// FINDOUT: Without std::clamp() ?
Cone::Cone(const Transform *ObjectToWorld, const Transform *WorldToObject, bool reverseOrientation,
           fp_t radius, fp_t height, fp_t phiMax)
    : Shape(ObjectToWorld, WorldToObject, reverseOrientation)
    , m_radius(radius)
    , m_height(height)
    , m_phiMax(pbr::Radians(std::clamp(phiMax, fp_t(0), fp_t(360))))
{}


// ---------------------------------------
// --------------- METHODS ---------------
// ---------------------------------------

Bounds3_t Cone::ObjectBound() const
{
    return Bounds3_t(Point3_t(-m_radius, -m_radius, 0),
                     Point3_t( m_radius,  m_radius, m_height));
}

// NOTE: testAlphaTexture is not used
bool Cone::Intersect(const Ray_arg r,
                     fp_t &out_tHit, SurfaceInteraction &out_isect,
                     bool /*testAlphaTexture = true*/) const
{
    PBR_PROFILE_FUNCTION(ProfileCategory::Shape_Intersect)

    // Transform ray to object space
    Vector3_t oError, dError;
    Ray ray = (*WorldToObject)(r, oError, dError);

    // Initialize EFloat ray coordinate values;
    EFloat ox(ray.origin.x, oError.x), oy(ray.origin.y, oError.y), oz(ray.origin.z, oError.z);
    EFloat dx(ray.direction.x, dError.x), dy(ray.direction.y, dError.y), dz(ray.direction.z, dError.z);
    EFloat e_height(m_height);
    EFloat k = EFloat(m_radius) / e_height;
    k = k * k;
    // Compute quadratic tConeHit coefficients
    EFloat a = dx * dx + dy * dy - k * dz * dz;
    EFloat b = 2 * (dx * ox + dy * oy - k * dz * (oz - e_height));
    EFloat c = ox * ox + oy * oy - k * (oz - e_height) * (oz - e_height);

    // Solve quadratic equation
    EFloat t0, t1;
    if (Quadratic(a, b, c, t0, t1) == false)
        return false;
    // After Quadratic() t0 will be <= t1, so only need to check this 2 conditions
    if (t0.UpperBound() > ray.tMax || t1.LowerBound() <= 0)
        return false;
    EFloat tConeHit = t0;
    if (tConeHit.LowerBound() <= 0) {
        tConeHit = t1;
        if (tConeHit.UpperBound() > ray.tMax)
            return false;
    }
   // Compute cone hit(intersection) point and phi
    Point3_t pHit = ray(fp_t(tConeHit));
    fp_t phi = pbr::ATan2(pHit.y, pHit.x);
    if (phi < 0) phi += constants::pi_t * 2;

    // TODO: This shit is repetitive and definitely can be optimized
    // Test cone intersection against clipping parametrs
    if (pHit.z < 0 || pHit.z > m_height || phi > m_phiMax) {
        // DIFFERENCE: Why the fuck they changed this check in all shapes after sphere in the book?
        if (tConeHit == t1 || t1.UpperBound() > ray.tMax)
            return false;
        tConeHit = t1;
        // Compute cone hit(intersection) point and phi
        pHit = ray(fp_t(tConeHit));
        phi = pbr::ATan2(pHit.y, pHit.x);
        if (phi < 0) phi += constants::pi_t * 2;
        if (pHit.z < 0 || pHit.z > m_height || phi > m_phiMax)
            return false;
    }

    // Find parametric representation of cone hit
    fp_t u = phi / m_phiMax;
    fp_t v = pHit.z / m_height;
    // Compute cone dpdu and dpdv
    Vector3_t dpdu(-m_phiMax * pHit.y, m_phiMax * pHit.x, 0);
    Vector3_t dpdv(-pHit.x / (fp_t(1) - v), -pHit.y / (fp_t(1) - v), m_height);

    // NOTE: This stuff(Partial derivatives of Noprmal Vectors, Weingarten equations) I don't understand.
    // Compute cone derivatives
    Vector3_t d2pduu = -m_phiMax * m_phiMax * Vector3_t(pHit.x, pHit.y, 0);
    Vector3_t d2pduv = m_phiMax / (fp_t(1) - v) * Vector3_t(pHit.y, -pHit.x, 0);
    Vector3_t d2pdvv(0);
    // Compute coefficients for fundamental forms
    // TODO: Since d2pdvv = 0 this thing can be simplified.
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
    // Compute error bounds for cone intersection
    EFloat px = ox + tConeHit * dx;
    EFloat py = oy + tConeHit * dy;
    EFloat pz = oz + tConeHit * dz;
    Vector3_t pError(px.GetAbsoluteError(), py.GetAbsoluteError(), pz.GetAbsoluteError());
//#endif

    out_tHit = fp_t(tConeHit);
    out_isect = (*ObjectToWorld)(SurfaceInteraction(pHit, pError,
                                                    Point2_t(u, v), -ray.direction,
                                                    dpdu, dpdv, dndu, dndv,
                                                    ray.time, this));

    return true;
}

// NOTE: testAlphaTexture is not used
bool Cone::IsIntersecting(const Ray_arg r, bool /*testAlphaTexture = true*/) const
{
    PBR_PROFILE_FUNCTION(ProfileCategory::Shape_IsIntersecting)

    // Transform ray to object space
    Vector3_t oError, dError;
    Ray ray = (*WorldToObject)(r, oError, dError);

    // Initialize EFloat ray coordinate values;
    EFloat ox(ray.origin.x, oError.x), oy(ray.origin.y, oError.y), oz(ray.origin.z, oError.z);
    EFloat dx(ray.direction.x, dError.x), dy(ray.direction.y, dError.y), dz(ray.direction.z, dError.z);
    EFloat e_height(m_height);
    EFloat k = EFloat(m_radius) / e_height;
    k = k * k;
    // Compute quadratic tConeHit coefficients
    EFloat a = dx * dx + dy * dy - k * dz * dz;
    EFloat b = 2 * (dx * ox + dy * oy - k * dz * (oz - e_height));
    EFloat c = ox * ox + oy * oy - k * (oz - e_height) * (oz - e_height);

    // Solve quadratic equation
    EFloat t0, t1;
    if (Quadratic(a, b, c, t0, t1) == false)
        return false;
    // After Quadratic() t0 will be <= t1, so only need to check this 2 conditions
    if (t0.UpperBound() > ray.tMax || t1.LowerBound() <= 0)
        return false;
    EFloat tConeHit = t0;
    if (tConeHit.LowerBound() <= 0) {
        tConeHit = t1;
        if (tConeHit.UpperBound() > ray.tMax)
            return false;
    }
   // Compute cone hit(intersection) point and phi
    Point3_t pHit = ray(fp_t(tConeHit));
    fp_t phi = pbr::ATan2(pHit.y, pHit.x);
    if (phi < 0) phi += constants::pi_t * 2;

    // TODO: This shit is repetitive and definitely can be optimized
    // Test cone intersection against clipping parametrs
    if (pHit.z < 0 || pHit.z > m_height || phi > m_phiMax) {
        // DIFFERENCE: Why the fuck they changed this check in all shapes after sphere in the book?
        if (tConeHit == t1 || t1.UpperBound() > ray.tMax)
            return false;
        tConeHit = t1;
        // Compute cone hit(intersection) point and phi
        pHit = ray(fp_t(tConeHit));
        phi = pbr::ATan2(pHit.y, pHit.x);
        if (phi < 0) phi += constants::pi_t * 2;
        if (pHit.z < 0 || pHit.z > m_height || phi > m_phiMax)
            return false;
    }

    return true;
}

// NOTE: Doesn't count base(circle) area
fp_t Cone::Area() const
{
    return m_radius * pbr::Sqrt(m_height * m_height + m_radius * m_radius) * m_phiMax / 2;
}

PBR_NAMESPACE_END
