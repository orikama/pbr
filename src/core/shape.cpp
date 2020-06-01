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
    ++stats_ShapesCreated;
}

} // namespace pbr
