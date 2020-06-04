#pragma once

#include "core.hpp"

#include "geometry.hpp"
#include "interaction.hpp"
#include "transform.hpp"

// TODO: Forward declarations ? Move all headers to .cpp

namespace pbr {

// NOTE: Why not all virtual methods deleted( =0 ) ?
// NOTE: Delete pointers in destructor ? (probably not, since Transform* shared across shapes)
class Shape
{
public:
    Shape(const Transform *ObjectToWorld, const Transform *WorldToObject,
          bool reverseOrientation);
    virtual ~Shape() = default;


    virtual Bounds3_t ObjectBound() const = 0;
    virtual Bounds3_t WorldBound() const;

    // NOTE: Questionable pointers as always, and naming.
    virtual bool Intersect(const Ray_arg ray,
                           fp_t *out_tHit, SurfaceInteraction *out_isect,
                           bool testAlphaTexture = true) const = 0;
    // NOTE: Why this method is defined in abstract class what the use of it ?
    // NOTE: Why use different name, why not just Intersect() ? Should be something like IsIntersecting()
    virtual bool IntersectP(const Ray_arg ray, bool testAlphaTexture = true) const;

    // Surface area of a shape in object space.
    virtual fp_t Area() const = 0;

// FINDOUT: Why not private ?
    const Transform *ObjectToWorld, *WorldToObject;
    const bool reverseOrientation;
    const bool transformSwapsHandedness;
};

} // namespace pbr
