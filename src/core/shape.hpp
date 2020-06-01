#pragma once

#include "core.hpp"

#include "geometry.hpp"
#include "interaction.hpp"
#include "transform.hpp"


namespace pbr {

// NOTE: Why not all virtual methods deleted( =0 ) ?
// NOTE: Delete pointers in destructor ?
class Shape
{
public:
    Shape(const Transform *ObjectToWorld, const Transform *WorldToObject,
          bool reverseOrientation);
    virtual ~Shape() = default;

    virtual Bounds3_t ObjectBound() const = 0;
    virtual Bounds3_t WorldBound() const;

    virtual bool Intersect(const Ray_arg ray, fp_t *tHit)

    virtual fp_t Area() = 0;

    const Transform *ObjectToWorld, *WorldToObject;
    const bool reverseOrientation;
    const bool transformSwapsHandedness;
};


// ---------------------------------------
// ------------ CONSTRUCTORS -------------
// ---------------------------------------

Shape::Shape(const Transform *ObjectToWorld, const Transform *WorldToObject,
             bool reverseOrientation)
    : ObjectToWorld(ObjectToWorld)
    , WorldToObject(WorldToObject)
    , reverseOrientation(reverseOrientation)
    , transformSwapsHandedness(ObjectToWorld->SwapsHandedness())
{}

} // namespace pbr
