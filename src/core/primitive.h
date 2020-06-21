#include "shape.h"
#include <memory>


// TODO: AreaLight, Material, MemoryArena, TransportMode not implemented.
// TODO: TransformedPrimitive not implemented, because I skipped Animated Transformations.


PBR_NAMESPACE_BEGIN

// NOTE: Name it IPrimitive ?
class Primitive
{
public:
    virtual ~Primitive() = default;
    virtual Bounds3_t WorldBound() const = 0;
    virtual bool Intersect(const Ray &out_r, SurfaceInteraction &out_isect) const = 0;
    virtual bool IsIntersecting(const Ray_arg r) const = 0;
    //virtual const AreaLight* GetAreaLight() const = 0;
    //virtual const Material* GetMaterial() const = 0;
    //virtual void ComputeScatteringFunction(SurfaceInteraction *isect,
    //                                       MemoryArena &arena,
    //                                       TransportMode mode,
    //                                       bool allowMultipleLobes) const = 0;
};


class GeometricPrimitive : public Primitive
{
public:
    GeometricPrimitive(const std::shared_ptr<Shape> &shape/*,
                       const std::shared_ptr<Material> &material,
                       const std::shared_ptr<AreaLight> &areaLight,
                       const MediumInterface &mediumInterface*/);

    Bounds3_t WorldBound() const override;
    bool Intersect(const Ray &out_r, SurfaceInteraction &out_isect) const override;
    bool IsIntersecting(const Ray_arg r) const override;
    // Returns a pointer that describes the primitive's emission distribution, if the primitive is a light source.
    //    Returns nullptr if the primitive is not emissive.
    //const AreaLight* GetAreaLight() const override;
    // If returns nullptr, ray intersections with the primitive should be ignored; in this case the primitive only serves
    //   to delineate volume of space for participating media. It's also used to check if two rays have intersected the same object.
    //const Material* GetMaterial() const override;
    //void ComputeScatteringFunction(SurfaceInteraction *isect,
    //                               MemoryArena &arena,
    //                               TransportMode mode,
    //                               bool allowMultipleLobes) const override;

private:
    // IMPROVE: And again this shared_ptr's, but in this case they're probably necessary.
    std::shared_ptr<Shape> m_shape;
    // IMPROVE: Optional field, same as with AreaLight ?
    //std::shared_ptr<Material> m_material;
    // IMPROVE: Optional field, a way to make more specialized classes. Separate GeometricPrimitive to LightSource or smth like that, to save memory ?
    //std::shared_ptr<AreaLight> m_areaLight;
    //MediumInterface m_mediumInterface;
};


//class TransformedPrimitive : public Primitive
//{};


// TODO: I don't know what to do with this class, it overrides this methods just to throw an error when they are called.
//       Is there a way to get rid of inheritance(composition ?), or should it be private inheritance, or mark them as =delete ?
//       I don't know what else, since I don't know how this class is used for now. It look like just another interface for acceleration structures.
class Aggregate : public Primitive
{
public:
    //const AreaLight* GetAreaLight() const override;
    //const Material* GetMaterial() const override;
    //void ComputeScatteringFunction(SurfaceInteraction *isect,
    //                               MemoryArena &arena,
    //                               TransportMode mode,
    //                               bool allowMultipleLobes) const override;
};

PBR_NAMESPACE_END
