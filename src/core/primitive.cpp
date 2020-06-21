#include "primitive.h"
#include "stats.h"


PBR_NAMESPACE_BEGIN

PBR_STATS_MEMORY_COUNTER("Memory/Primitives", stats_Primitive_bytes)


// ******************************************************************************
// ----------------------------- GeometricPrimitive -----------------------------
// ******************************************************************************

GeometricPrimitive::GeometricPrimitive(const std::shared_ptr<Shape> &shape/*,
                                       const std::shared_ptr<Material> &material,
                                       const std::shared_ptr<AreaLight> &areaLight,
                                       const MediumInterface &mediumInterface*/)
    : m_shape(shape)
    //, m_material(material)
    //, m_areaLight(areaLight)
    //, m_mediumInterface(mediumInterface)
{
    PBR_STATS_VARIABLE_ADD(stats_Primitive_bytes, sizeof(*this))
}

Bounds3_t GeometricPrimitive::WorldBound() const
{
    return m_shape->WorldBound();
}

// TODO: MediumInterface not implemented
bool GeometricPrimitive::Intersect(const Ray &out_r, SurfaceInteraction &out_isect) const
{
    fp_t tHit;

    if (m_shape->Intersect(out_r, tHit, out_isect)) {
        out_r.tMax = tHit;

        out_isect.primitive = this;
        // NOTE: Why this check exist ?
        PBR_ASSERT(Dot(out_isect.normal, out_isect.shading.normal) >= 0)
        //if (m_mediumInterface.IsmediumTransition())
        //else

        return true;
    }

    return false;
}

bool GeometricPrimitive::IsIntersecting(const Ray_arg r) const
{
    return m_shape->IsIntersecting(r);
}


// ******************************************************************************
// ---------------------------- TransformedPrimitive ----------------------------
// ******************************************************************************


// ******************************************************************************
// --------------------------------- Aggregate ----------------------------------
// ******************************************************************************


PBR_NAMESPACE_END
