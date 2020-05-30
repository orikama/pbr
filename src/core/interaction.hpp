#pragma once

#include "core.hpp"
#include "geometry.hpp"


// TODO: Separate implementation in .cpp file ?

namespace pbr {

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
    Vector3_t wo;       // outgoing direction
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

// TODO: Shape not implemented
// TODO: BSDF and BSSRDF not implemented
// NOTE: i32 faceIndex was not implemented int the book
// NOTE: Empty constructor ?
struct SurfaceInteraction : public Interaction
{

    SurfaceInteraction(const Point3_arg<fp_t> p, const Vector3_arg<fp_t> pError,
                       const Point2_arg<fp_t> uv, const Vector3_arg<fp_t> wo,
                       const Vector3_arg<fp_t> dpdu, const Vector3_arg<fp_t> dpdv,
                       const Normal3_arg<fp_t> dndu, const Normal3_arg<fp_t> dndv,
                       fp_t time
                       //const Shape *sh,
                       /*int faceIndex = 0*/);

    void SetShadingGeometry(const Vector3_arg<fp_t> dpdu, const Vector3_arg<fp_t> dpdv,
                            const Normal3_arg<fp_t> dndu, const Normal3_arg<fp_t> dndv,
                            bool orientationIsAuthoritative);

    Point2_t uv;
    Vector3_t dpdu, dpdv;
    Normal3_t dndu, dndv;
    //const Shape *shape = nullptr;
    struct {
        Normal3_t n;
        Vector3_t dpdu, dpdv;
        Normal3_t dndu, dndv;
    } Shading;
    //const Primitive *primitive = nullptr;
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
                                       fp_t time
                                       //const Shape *sh,
                                       /*int faceIndex = 0*/)
                                       {}


// ---------------------------------------
// ---------- UTILITY FUNCTIONS ----------
// ---------------------------------------


#pragma endregion SurfaceInteraction

} // namespace pbr
