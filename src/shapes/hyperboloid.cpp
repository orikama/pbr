#include "hyperboloid.h"
#include "../core/stats.h"
#include "../core/efloat.hpp"


PBR_NAMESPACE_BEGIN

// ---------------------------------------
// ------------ CONSTRUCTORS -------------
// ---------------------------------------

// FINDOUT: Without std::clamp() and min()/max() ?
// Hyperboloid::Hyperboloid(const Transform *ObjectToWorld, const Transform *WorldToObject, bool reverseOrientation,
//                          Point3_arg<fp_t> point1, Point3_arg<fp_t> point2, fp_t tm)
//     : Shape(ObjectToWorld, WorldToObject, reverseOrientation)
//     , m_point1(point1)
//     , m_point2(point2)
//     , m_zMin(std::min(point1.z, point2.z))
//     , m_zMax(std::max(point1.z, point2.z))
//     , m_phiMax(pbr::Radians(std::clamp(tm, fp_t(0), fp_t(360))))
// {
//     fp_t radius1 = pbr::Sqrt(point1.x * point1.x + point1.y * point1.y);
//     fp_t radius2 = pbr::Sqrt(point2.x * point2.x + point2.y * point2.y);
//     m_rMax = std::max(radius1, radius2);


// }


// ---------------------------------------
// --------------- METHODS ---------------
// ---------------------------------------

// Bounds3_t Hyperboloid::ObjectBound() const
// {
//     // NOTE: Is this the answer for sphere bounds computation excersice ?
//     return Bounds3_t(Point3_t(-m_rMax, -m_rMax, m_zMin),
//                      Point3_t( m_rMax,  m_rMax, m_zMax));
// }

// NOTE: testAlphaTexture is not used
// bool Paraboloid::Intersect(const Ray_arg r,
//                            fp_t &out_tHit, SurfaceInteraction &out_isect,
//                            bool /*testAlphaTexture = true*/) const
// {
//     PBR_PROFILE_FUNCTION(ProfileCategory::Shape_Intersect)

//     // Transform ray to object space
//     Vector3_t oError, dError;
//     Ray ray = (*WorldToObject)(r, oError, dError);

//     // Initialize EFloat ray coordinate values;
//     EFloat ox(ray.origin.x, oError.x), oy(ray.origin.y, oError.y), oz(ray.origin.z, oError.z);
//     EFloat dx(ray.direction.x, dError.x), dy(ray.direction.y, dError.y), dz(ray.direction.z, dError.z);
//     EFloat k = EFloat(m_zMax) / EFloat(m_radius) * EFloat(m_radius);
//     // Compute quadratic tParaboloidHit coefficients
//     EFloat a = k * (dx * dx + dy * dy);
//     EFloat b = 2 * k * (dx * ox + dy * oy) - dz;
//     EFloat c = k * (ox * ox + oy * oy) - oz;

//     // Solve quadratic equation
//     EFloat t0, t1;
//     if (Quadratic(a, b, c, t0, t1) == false)
//         return false;
//     // After Quadratic() t0 will be <= t1, so only need to check this 2 conditions
//     if (t0.UpperBound() > ray.tMax || t1.LowerBound() <= 0)
//         return false;
//     EFloat tParaboloidHit = t0;
//     if (tParaboloidHit.LowerBound() <= 0) {
//         tParaboloidHit = t1;
//         if (tParaboloidHit.UpperBound() > ray.tMax)
//             return false;
//     }
//    // Compute paraboloid hit(intersection) point and phi
//     Point3_t pHit = ray(fp_t(tParaboloidHit));
//     fp_t phi = pbr::ATan2(pHit.y, pHit.x);
//     if (phi < 0) phi += constants::pi_t * 2;

//     // TODO: This shit is repetitive and definitely can be optimized
//     // Test paraboloid intersection against clipping parametrs
//     if (pHit.z < m_zMin || pHit.z > m_zMax || phi > m_phiMax) {
//         // DIFFERENCE: Why the fuck they changed this check in all shapes after sphere in the book?
//         if (tParaboloidHit == t1 || t1.UpperBound() > ray.tMax)
//             return false;
//         tParaboloidHit = t1;
//         // Compute cone hit(intersection) point and phi
//         pHit = ray(fp_t(tParaboloidHit));
//         phi = pbr::ATan2(pHit.y, pHit.x);
//         if (phi < 0) phi += constants::pi_t * 2;
//         if (pHit.z < m_zMin || pHit.z > m_zMax || phi > m_phiMax)
//             return false;
//     }

//     const fp_t zDiff = m_zMax - m_zMin;
//     const fp_t pHitZ2 = 2 * pHit.z;
//     // Find parametric representation of paraboloid hit
//     fp_t u = phi / m_phiMax;
//     fp_t v = (pHit.z - m_zMin) / zDiff;
//     // Compute paraboloid dpdu and dpdv
//     Vector3_t dpdu(-m_phiMax * pHit.y, m_phiMax * pHit.x, 0);
//     Vector3_t dpdv = zDiff * Vector3_t(pHit.x / pHitZ2, pHit.y / pHitZ2, 1);

//     // NOTE: This stuff(Partial derivatives of Noprmal Vectors, Weingarten equations) I don't understand.
//     // Compute paraboloid derivatives
//     Vector3_t d2pduu = -m_phiMax * m_phiMax * Vector3_t(pHit.x, pHit.y, 0);
//     Vector3_t d2pduv = zDiff * m_phiMax * Vector3_t(-pHit.y / pHitZ2, pHit.x / pHitZ2, 0);
//     Vector3_t d2pdvv = -zDiff * zDiff * Vector3_t(-pHit.x / (pHitZ2 * pHitZ2), pHit.y / (pHitZ2 * pHitZ2), 0);
//     // Compute coefficients for fundamental forms
//     fp_t E = Dot(dpdu, dpdu);
//     fp_t F = Dot(dpdu, dpdv);
//     fp_t G = Dot(dpdv, dpdv);
//     Vector3_t N = Normalize(Cross(dpdu, dpdv)); // NOTE: This vector will be computed again in SurfaceInteraction constructor.
//     fp_t e = Dot(N, d2pduu);
//     fp_t f = Dot(N, d2pduv);
//     fp_t g = Dot(N, d2pdvv);
//     // Compute Partial derivatives of Noprmal Vectors from fundamental form coefficients
//     fp_t invEGF2 = fp_t(1) / (E * G - F * F);
//     Normal3_t dndu((f * F - e * G) * invEGF2 * dpdu + (e * F - f * E) * invEGF2 * dpdv);
//     Normal3_t dndv((g * F - f * G) * invEGF2 * dpdu + (f * F - g * E) * invEGF2 * dpdv);

//     // NOTE: This thing is nessesary for SurfaceConstruction anyway.
// //#if PBR_PBR_ENABLE_EFLOAT == 1
//     // Compute error bounds for paraboloid intersection
//     EFloat px = ox + tParaboloidHit * dx;
//     EFloat py = oy + tParaboloidHit * dy;
//     EFloat pz = oz + tParaboloidHit * dz;
//     Vector3_t pError(px.GetAbsoluteError(), py.GetAbsoluteError(), pz.GetAbsoluteError());
// //#endif

//     out_tHit = fp_t(tParaboloidHit);
//     out_isect = (*ObjectToWorld)(SurfaceInteraction(pHit, pError,
//                                                     Point2_t(u, v), -ray.direction,
//                                                     dpdu, dpdv, dndu, dndv,
//                                                     ray.time, this));

//     return true;
// }

// // NOTE: testAlphaTexture is not used
// bool Paraboloid::IsIntersecting(const Ray_arg r, bool /*testAlphaTexture = true*/) const
// {
//     PBR_PROFILE_FUNCTION(ProfileCategory::Shape_IsIntersecting)

//     // Transform ray to object space
//     Vector3_t oError, dError;
//     Ray ray = (*WorldToObject)(r, oError, dError);

//     // Initialize EFloat ray coordinate values;
//     EFloat ox(ray.origin.x, oError.x), oy(ray.origin.y, oError.y), oz(ray.origin.z, oError.z);
//     EFloat dx(ray.direction.x, dError.x), dy(ray.direction.y, dError.y), dz(ray.direction.z, dError.z);
//     EFloat k = EFloat(m_zMax) / EFloat(m_radius) * EFloat(m_radius);
//     // Compute quadratic tParaboloidHit coefficients
//     EFloat a = k * (dx * dx + dy * dy);
//     EFloat b = 2 * k * (dx * ox + dy * oy) - dz;
//     EFloat c = k * (ox * ox + oy * oy) - oz;

//     // Solve quadratic equation
//     EFloat t0, t1;
//     if (Quadratic(a, b, c, t0, t1) == false)
//         return false;
//     // After Quadratic() t0 will be <= t1, so only need to check this 2 conditions
//     if (t0.UpperBound() > ray.tMax || t1.LowerBound() <= 0)
//         return false;
//     EFloat tParaboloidHit = t0;
//     if (tParaboloidHit.LowerBound() <= 0) {
//         tParaboloidHit = t1;
//         if (tParaboloidHit.UpperBound() > ray.tMax)
//             return false;
//     }
//    // Compute paraboloid hit(intersection) point and phi
//     Point3_t pHit = ray(fp_t(tParaboloidHit));
//     fp_t phi = pbr::ATan2(pHit.y, pHit.x);
//     if (phi < 0) phi += constants::pi_t * 2;

//     // TODO: This shit is repetitive and definitely can be optimized
//     // Test paraboloid intersection against clipping parametrs
//     if (pHit.z < m_zMin || pHit.z > m_zMax || phi > m_phiMax) {
//         // DIFFERENCE: Why the fuck they changed this check in all shapes after sphere in the book?
//         if (tParaboloidHit == t1 || t1.UpperBound() > ray.tMax)
//             return false;
//         tParaboloidHit = t1;
//         // Compute cone hit(intersection) point and phi
//         pHit = ray(fp_t(tParaboloidHit));
//         phi = pbr::ATan2(pHit.y, pHit.x);
//         if (phi < 0) phi += constants::pi_t * 2;
//         if (pHit.z < m_zMin || pHit.z > m_zMax || phi > m_phiMax)
//             return false;
//     }

//     return true;
// }

// // NOTE: Not verified.
// fp_t Paraboloid::Area() const
// {
//     fp_t radius2 = m_radius * m_radius;
//     fp_t k = 4 * m_zMax / radius2;
//     return (radius2 * radius2 * m_phiMax / (12 * m_zMax * m_zMax)) *
//            (std::pow(k * m_zMax + 1, fp_t(1.5)) - std::pow(k * m_zMin + 1, fp_t(1.5)));
// }

PBR_NAMESPACE_END
