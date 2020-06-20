#include "shape.h"
#include "stats.h"


PBR_NAMESPACE_BEGIN

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
    PBR_STATS_VARIABLE_INCREMENT(stats_ShapesCreated)
}


// ---------------------------------------
// --------------- METHODS ---------------
// ---------------------------------------

Bounds3_t Shape::WorldBound() const
{
    return (*ObjectToWorld)(ObjectBound());
}

bool Shape::IsIntersecting(const Ray_arg r, bool testAlphaTexture = true) const
{
    // DIFFERENCE: I changed Intersect(const Ray_arg, fp_t &, SurfaceInteraction &,bool) to to references instead of pointers.
    //             So I need to rework this function. 
    return Intersect(r, nullptr, nullptr, testAlphaTexture);
}


PBR_NAMESPACE_END
