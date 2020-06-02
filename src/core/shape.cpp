#include "shape.h"
#include "stats.h"


namespace pbr {

// ---------------------------------------
// ------------ CONSTRUCTORS -------------
// ---------------------------------------

PBR_STATS_COUNTER("Scene/Shapes created", stats_ShapesCreated)
Shape::Shape(const Transform *ObjectToWorld, const Transform *WorldToObject,
             bool reverseOrientation)
    : ObjectToWorld(ObjectToWorld)
    , WorldToObject(WorldToObject)
    , reverseOrientation(reverseOrientation)
    , transformSwapsHandedness(ObjectToWorld->SwapsHandedness())
{
    PBR_STATS_COUNTER_INCREMENT(stats_ShapesCreated)
}


// ---------------------------------------
// --------------- METHODS ---------------
// ---------------------------------------

Bounds3_t Shape::WorldBound() const
{
    return (*ObjectToWorld)(ObjectBound());
}

bool Shape::IntersectP(const Ray_arg ray, bool testAlphaTexture = true) const
{
    return Intersect(ray, nullptr, nullptr, testAlphaTexture);
}


} // namespace pbr
