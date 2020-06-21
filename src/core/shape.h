#pragma once

#include "core.hpp"

#include "geometry.hpp"
#include "interaction.hpp"
#include "transform.hpp"

// TODO: Forward declarations ? Move all headers to .cpp

PBR_NAMESPACE_BEGIN

// NOTE: Why not all virtual methods deleted( =0 ) ?
//       Because only Triangle implements WorldBound. Don't know about IsIntersecting() for now.
// NOTE: Delete pointers in destructor ? (probably not, since Transform* shared across shapes)
class Shape
{
public:
    Shape(const Transform *ObjectToWorld, const Transform *WorldToObject, bool reverseOrientation);
    virtual ~Shape() = default;


    virtual Bounds3_t ObjectBound() const = 0;
    virtual Bounds3_t WorldBound() const;

    // NOTE: Questionable pointers as always, and naming.
    virtual bool Intersect(const Ray_arg r,
                           fp_t &out_tHit, SurfaceInteraction &out_isect,
                           bool testAlphaTexture = true) const = 0;
    // NOTE: Why this method is defined in abstract class what the use of it ?
    virtual bool IsIntersecting(const Ray_arg r, bool testAlphaTexture = true) const;

    // Surface area of a shape in object space.
    virtual fp_t Area() const = 0;


// FINDOUT: Why not private ? I'm sure that it can be at least protected.
    const Transform *ObjectToWorld, *WorldToObject;
    const bool reverseOrientation;
    const bool transformSwapsHandedness;
};

PBR_NAMESPACE_END
