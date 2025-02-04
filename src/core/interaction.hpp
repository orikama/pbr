#pragma once

#include "core.hpp"
#include "geometry.hpp"
#include "primitive.h"


// TODO: Separate implementation in .cpp file ?

PBR_NAMESPACE_BEGIN

// ******************************************************************************
// -------------------------------- INTERACTION ---------------------------------
// ******************************************************************************

#pragma region Interaction

// TODO: MediumInterface not implemented
// NOTE: Empty constructor ?
struct Interaction
{
    explicit Interaction(const Point3_arg<fp_t> point,
                         const Normal3_arg<fp_t> normal,
                         const Vector3_arg<fp_t> pError,
                         const Vector3_arg<fp_t> wo,
                         fp_t time /*, const MediumInterface &mediumInterface*/);

    explicit Interaction(const Point3_arg<fp_t> point,
                         const Vector3_arg<fp_t> wo,
                         fp_t time /*, const MediumInterface &mediumInterface*/);

    explicit Interaction(const Point3_arg<fp_t> point,
                         fp_t time
                         /*const MediumInterface &mediumInterface*/);


    bool IsSurfaceInteraction() const;


    Point3_t point;
    Vector3_t pError;
    Vector3_t wo;       // negative ray direction, outgoing direction when computing lightning at point
    Normal3_t normal;   // surface normal at the point
    //MediumInterface mediumInterface;
    fp_t time;
};


// ---------------------------------------
// ------------ CONSTRUCTORS -------------
// ---------------------------------------

Interaction::Interaction(const Point3_arg<fp_t> point,
                         const Normal3_arg<fp_t> normal,
                         const Vector3_arg<fp_t> pError,
                         const Vector3_arg<fp_t> wo,
                         fp_t time /*, const MediumInterface &mediumInterface*/)
    : point(point)
    , normal(normal)
    , pError(pError)
    , wo(Normalize(wo))
    , time(time)
    //, mediumInterface(mediumInterface)
{}

Interaction::Interaction(const Point3_arg<fp_t> point,
                         const Vector3_arg<fp_t> wo,
                         fp_t time /*, const MediumInterface &mediumInterface*/)
    : point(point)
    , time(time)
    , wo(wo)
    //, mediumInterface(mediumInterface)
{}

Interaction::Interaction(const Point3_arg<fp_t> point,
                         fp_t time
                         /*const MediumInterface &mediumInterface*/)
    : point(point)
    , time(time)
    //, mediumInterface(mediumInterface)
{}


// ---------------------------------------
// ---------- UTILITY FUNCTIONS ----------
// ---------------------------------------

// NOTE: Is this will generate correct assembly?
//       or should I write it straightforward: notrmal.x != 0 || normal.y != 0 || normal.z != 0
bool Interaction::IsSurfaceInteraction() const
{ 
    return normal != Normal3_t();
}


#pragma region MediumIntercation


// ******************************************************************************
// ----------------------------- MEDIUMINTERCATION ------------------------------
// ******************************************************************************

#pragma region MediumIntercation

// TODO: MediumInterface not implemented
struct MediumIntercation : public Interaction
{
};

#pragma endregion MediumIntercation


// ******************************************************************************
// ----------------------------- SURFACEINTERACTION -----------------------------
// ******************************************************************************

#pragma region SurfaceInteraction

// TODO: BSDF and BSSRDF not implemented
// TODO: Primitive not implemented
// NOTE: i32 faceIndex was not implemented in the book
// IMPROVE: This struct is huge and I think half of its stuff will be not used in some cases, may be I need to separate it.
//       For example Shape::Intersect methods using only first half of its fields, and then they are populated manually,
//       by functions that calling this Intersect methods.
// DIFFERENCE: No empty constructor.
struct SurfaceInteraction : public Interaction
{

    explicit SurfaceInteraction(const Point3_arg<fp_t> point, const Vector3_arg<fp_t> pError,
                                const Point2_arg<fp_t> uv, const Vector3_arg<fp_t> wo,
                                const Vector3_arg<fp_t> dpdu, const Vector3_arg<fp_t> dpdv,
                                const Normal3_arg<fp_t> dndu, const Normal3_arg<fp_t> dndv,
                                fp_t time,
                                const Shape *shape
                                /*int faceIndex = 0*/);

    //void SetShadingGeometry(const Vector3_arg<fp_t> dpdu, const Vector3_arg<fp_t> dpdv,
    //                        const Normal3_arg<fp_t> dndu, const Normal3_arg<fp_t> dndv,
    //                        bool orientationIsAuthoritative);


    Point2_t uv;
    Vector3_t dpdu, dpdv;
    Normal3_t dndu, dndv;
    const Shape *shape = nullptr;
    struct {
        Normal3_t normal;
        Vector3_t dpdu, dpdv;
        Normal3_t dndu, dndv;
    } shading;

    const Primitive *primitive = nullptr;
    //BSDF *bsdf = nullptr;
    //BSSRDF *bssrdf = nullptr;

    // NOTE: next 2 lines marked as mutable in original
    Vector3_t dpdx, dpdy;
    fp_t dudx = 0, dvdx = 0, dudy = 0, dvdy = 0;

    // Added after book publication. Shapes can optionally provide a face
    // index with an intersection point for use in Ptex texture lookups.
    // If Ptex isn't being used, then this value is ignored.
    //i32 faceIndex = 0;
};


// ---------------------------------------
// ------------ CONSTRUCTORS -------------
// ---------------------------------------

SurfaceInteraction::SurfaceInteraction(const Point3_arg<fp_t> point, const Vector3_arg<fp_t> pError,
                                       const Point2_arg<fp_t> uv, const Vector3_arg<fp_t> wo,
                                       const Vector3_arg<fp_t> dpdu, const Vector3_arg<fp_t> dpdv,
                                       const Normal3_arg<fp_t> dndu, const Normal3_arg<fp_t> dndv,
                                       fp_t time,
                                       const Shape *shape
                                       /*int faceIndex = 0*/)
    : Interaction(point, Normal3_t(Normalize(Cross(dpdu, dpdv))), pError, wo, time /*,nullptr*/)
    , uv(uv)
    , dpdu(dpdu)
    , dpdv(dpdv)
    , dndu(dndu)
    , dndv(dndv)
    , shape(shape)
    //, faceIndex(faceIndex)
{
    if (shape && (shape->reverseOrientation ^ shape->transformSwapsHandedness)) {
        normal *= -1;
    }
    shading.n = normal;
    shading.dpdu = dpdu;
    shading.dpdv = dpdv;
    shading.dndu = dndu;
    shading.dndv = dndv;
}


// ---------------------------------------
// --------------- METHODS ---------------
// ---------------------------------------

// FINDOUT: Why do the same work, that constructor already did ?
//void SurfaceInteraction::SetShadingGeometry(const Vector3_arg<fp_t> dpdu, const Vector3_arg<fp_t> dpdv,
//                                            const Normal3_arg<fp_t> dndu, const Normal3_arg<fp_t> dndv,
//                                            bool orientationIsAuthoritative)
//{
    
//}


#pragma endregion SurfaceInteraction

PBR_NAMESPACE_END
