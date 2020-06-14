#include "sphere.h"
#include "../core/stats.h"
#include "../core/efloat.hpp"


PBR_NAMESPACE_BEGIN

// ---------------------------------------
// ------------ CONSTRUCTORS -------------
// ---------------------------------------

// FINDOUT: Will compiler optimize this repetitive code ?
// NOTE: And why the fuck there is a need in min(), max() at all?
//       And what zMin/zMax will be used in thetaMin/Max initialization (since I changed naming of private fields there is no problem, but now it may work differently) ?
//       What's the use of std::clamp for phiMax, it should be error if phiMax < 0 or > 360, not clamp. Probably same with zMin/zMax, they shouldn't be more than radius.
// FINDOUT: If this is right - https://github.com/mmp/pbrt-v3/issues/259, I also need to change 'fp_t theta = std::acos(std::clamp())' line in Sphere::Intersect()
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

// NOTE: testAlphaTexture is not used
bool Sphere::Intersect(const Ray_arg r,
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
    // Compute quadratic sphere coefficients
    EFloat a = dx * dx + dy * dy + dz * dz;
    EFloat b = 2 * (dx * ox + dy * oy + dz * oz);
    EFloat c = ox * ox + oy * oy + oz * oz - EFloat(m_radius) * EFloat(m_radius);
    // Solve quadratic equation
    EFloat t0, t1;
    if (Quadratic(a, b, c, t0, t1) == false)
        return false;
    // After Quadratic() t0 will be <= t1, so only need to check this 2 conditions
    if (t0.UpperBound() > ray.tMax || t1.LowerBound() <= 0)
        return false;
    EFloat tSphereHit = t0;
    if (tSphereHit.LowerBound() <= 0) {
        tSphereHit = t1;
        if (tSphereHit.UpperBound() > ray.tMax)
            return false;
    }
    // Compute sphere hit(intersection) point and phi
    Point3_t pHit = ray((fp_t)tSphereHit);
    // Refine sphere intersection point
//#if PBR_PBR_ENABLE_EFLOAT == 1
    pHit *= m_radius / Distance(pHit, Point3_t(0)); // NOTE: This Distance call can be more effective, basically it's similiar to pHit.Length()
    // FINDOUT: I don't know what this line doing, there is not explanation in the book. Seems like it's safe guard for std::atan2()
    if (pHit.x == 0 && pHit.y == 0) pHit.x = 1e-5 * m_radius;
//#endif
    fp_t phi = pbr::ATan2(pHit.y, pHit.x);
    if (phi < 0) phi += constants::pi_t * 2;

    // TODO: This shit is repetitive and definitely can be optimized
    // Test sphere intersection against clipping parametrs
    if (m_zMin > -m_radius && pHit.z < m_zMin || m_zMax < m_radius && pHit.z > m_zMax || phi > m_phiMax) {
        if (tSphereHit == t1 || t1.UpperBound() > ray.tMax)
            return false;
        tSphereHit = t1;
        // Compute sphere hit(intersection) point and phi
        pHit = ray(fp_t(tSphereHit));
        pHit *= m_radius / Distance(pHit, Point3_t(0));
        if (pHit.x == 0 && pHit.y == 0) pHit.x = 1e-5 * m_radius;
        phi = pbr::ATan2(pHit.y, pHit.x);
        if (phi < 0) phi += constants::pi_t * 2;
        if (m_zMin > -m_radius && pHit.z < m_zMin || m_zMax < m_radius && pHit.z > m_zMax || phi > m_phiMax)
            return false;
    }

    // Find parametric representation of sphere hit
    fp_t u = phi / m_phiMax;
    fp_t theta = pbr::ACos(std::clamp(pHit.z / m_radius, fp_t(-1), fp_t(1)));
    fp_t v = (theta - m_thetaMin) / (m_thetaMax - m_thetaMin);
    // Compute sphere dpdu and dpdv
    // FINDOUT: Why the fuck cosPhi and sinPhi calculated manually, instead of using Sin(phi), Cos(phi) ?
    //          Most likely they tried to do what sincos() does. But I'm sure it will be slower, as there is a special instructions for this task.
    fp_t zRadius = pbr::Sqrt(pHit.x * pHit.x + pHit.y * pHit.y);
    fp_t invZRadius = 1 / zRadius;
    fp_t cosPhi = pHit.x * invZRadius;
    fp_t sinPhi = pHit.y * invZRadius;
    Vector3_t dpdu(-m_phiMax * pHit.y, m_phiMax * pHit.x, 0);
    Vector3_t dpdv = (m_thetaMax - m_thetaMin) * Vector3_t(pHit.z * cosPhi, pHit.z * sinPhi, -m_radius * pbr::Sin(theta));

    // NOTE: This stuff(Partial derivatives of Noprmal Vectors, Weingarten equations) I don't understand.
    // Compute second derivatives
    Vector3_t d2pduu = -m_phiMax * m_phiMax * Vector3_t(pHit.x, pHit.y, 0);
    Vector3_t d2pduv = (m_thetaMax - m_thetaMin) * pHit.z * m_phiMax * Vector3_t(-sinPhi, cosPhi, 0);
    Vector3_t d2pdvv = -(m_thetaMax - m_thetaMin) * (m_thetaMax - m_thetaMin) * Vector3_t(pHit.x, pHit.y, pHit.z);
    // Compute coefficients for fundamental forms
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
    Vector3_t pError(Gamma(5) * pbr::Abs(pHit));
//#endif

    out_tHit = fp_t(tSphereHit);
    out_isect = (*ObjectToWorld)(SurfaceInteraction(pHit, pError,
                                                    Point2_t(u, v), -ray.direction,
                                                    dpdu, dpdv, dndu, dndv,
                                                    ray.time, this));

    return true;
}

bool Sphere::IsIntersecting(const Ray_arg r, bool /*testAlphaTexture = true*/) const
{
    PBR_PROFILE_FUNCTION(ProfileCategory::Shape_IsIntersecting)

    // Transform ray to object space
    Vector3_t oError, dError;
    Ray ray = (*WorldToObject)(r, oError, dError);

    // Initialize EFloat ray coordinate values;
    EFloat ox(ray.origin.x, oError.x), oy(ray.origin.y, oError.y), oz(ray.origin.z, oError.z);
    EFloat dx(ray.direction.x, dError.x), dy(ray.direction.y, dError.y), dz(ray.direction.z, dError.z);
    // Compute quadratic sphere coefficients
    EFloat a = dx * dx + dy * dy + dz * dz;
    EFloat b = 2 * (dx * ox + dy * oy + dz * oz);
    EFloat c = ox * ox + oy * oy + oz * oz - EFloat(m_radius) * EFloat(m_radius);
    // Solve quadratic equation
    EFloat t0, t1;
    if (Quadratic(a, b, c, t0, t1) == false)
        return false;
    // After Quadratic() t0 will be <= t1, so only need to check this 2 conditions
    if (t0.UpperBound() > ray.tMax || t1.LowerBound() <= 0)
        return false;
    EFloat tSphereHit = t0;
    if (tSphereHit.LowerBound() <= 0) {
        tSphereHit = t1;
        if (tSphereHit.UpperBound() > ray.tMax)
            return false;
    }
    // Compute sphere hit(intersection) point and phi
    Point3_t pHit = ray((fp_t)tSphereHit);
    pHit *= m_radius / Distance(pHit, Point3_t(0)); // NOTE: This Distance call can be more effective, basically it's similiar to pHit.Length()
    // FINDOUT: I don't know what this line doing, there is not explanation in the book. Seems like it's safe guard for std::atan2()
    if (pHit.x == 0 && pHit.y == 0) pHit.x = 1e-5 * m_radius;
    fp_t phi = pbr::ATan2(pHit.y, pHit.x);
    if (phi < 0) phi += constants::pi_t * 2;

    // TODO: This shit is repetitive and definitely can be optimized
    // Test sphere intersection against clipping parametrs
    if (m_zMin > -m_radius && pHit.z < m_zMin || m_zMax < m_radius && pHit.z > m_zMax || phi > m_phiMax) {
        if (tSphereHit == t1 || t1.UpperBound() > ray.tMax)
            return false;
        tSphereHit = t1;
        // Compute sphere hit(intersection) point and phi
        Point3_t pHit = ray((fp_t)tSphereHit);
        pHit *= m_radius / Distance(pHit, Point3_t(0));
        if (pHit.x == 0 && pHit.y == 0) pHit.x = 1e-5 * m_radius;
        fp_t phi = pbr::ATan2(pHit.y, pHit.x);
        if (phi < 0) phi += constants::pi_t * 2;
        if (m_zMin > -m_radius && pHit.z < m_zMin || m_zMax < m_radius && pHit.z > m_zMax || phi > m_phiMax)
            return false;
    }

    return true;
}

fp_t Sphere::Area() const
{
    return m_phiMax * m_radius * (m_zMax - m_zMin);
}

PBR_NAMESPACE_END
