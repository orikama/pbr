#pragma once

#include "../core/shape.h"
#include <memory>
#include <vector>


PBR_NAMESPACE_BEGIN

// TODO: Texture not implemented
// TODO: Probably this arrays copyieng can be improved using std::array, std::move and I don't know what else. Passing this pointers looks like shit.
// TODO: May be I need to add move constructor and move assignment, and delete copy constructor and assignment operator just in case.
struct TriangleMesh
{
    // TODO: Interesting that there they used reference to ObjectToWorld instead of pointer. I should change it everywhere to use reference.
    TriangleMesh(const Transform &ObjectToWorld,
                 i32 _nTriangles, const i32 *_vertexIndices,
                 i32 _nVertices, const Point3_t *_positions,
                 const Vector3_t *_tangents, const Normal3_t *_normals, const Point2_t *_uv
                 //const std::shared_ptr<Texture<fp_t>> &alphaMask,
                 //const std::shared_ptr<Texture<fp_t>> &shadingMask,
                 /*const i32 *faceIndices*/);

    // TODO: Most likely std::array will be better than std::vector.
    const i32 nTriangles, nVertices;
    // A pointer to an array of vertex indices.
    std::vector<i32> vertexIndices;
    // An array of $nVertices$ vertex positions.
    std::unique_ptr<Point3_t[]> positions;
    // An optional array of normal vectors, one per vertex in the mesh. If present, these are interpolated across triangle faces to compute shading normals.
    std::unique_ptr<Normal3_t[]> normals;
    // An optional array of tangent vectors, one per vertex in the mesh. These are used to compute shading tangents.
    std::unique_ptr<Vector3_t[]> tangents;
    // An optional array of parametric(u,v) values, one per vertex.
    std::unique_ptr<Point2_t[]> uv;
    // An optional alpha mask texture, which can be used to cut away parts of triangle surfaces.
    //std::shared_ptr<Texture<fp_t>> alphaMask;
    //std::shared_ptr<Texture<fp_t>> shadowAlphaMask // DIFFERENCE: Was not presented in the book.
    //std::vector<i32> faceIndices; // DIFFERENCE: Was not presented in the book.
};


// NOTE: I think the whole structure is kinda bad, when we wasting memory, storing pointer to a TriangleMesh, in every triangle.
//       Shouldn't TriangleMesh just store array of triangles ?
class Triangle : public Shape
{
public:
    Triangle(const Transform *ObjectToWorld, const Transform *WorldToObject, bool reverseOrientation,
             const std::shared_ptr<TriangleMesh> &mesh, i32 triangleIndex);


    Bounds3_t ObjectBound() const override;
    Bounds3_t WorldBound() const override;

    bool Intersect(const Ray_arg r,
                   fp_t &out_tHit, SurfaceInteraction &out_isect,
                   bool testAlphaTexture = true) const override;
    bool IsIntersecting(const Ray_arg r, bool testAlphaTexture = true) const override;

    fp_t Area() const override;


private:
    // TODO: Return instead of pointer ?
    void GetUV(Point2_t out_uv[3]) const;


    std::shared_ptr<TriangleMesh> m_mesh;
    // Pointer to a three triangle vertex indices in the mesh.
    const i32 *m_vIndices;
    //i32 m_faceIndex; // DIFFERENCE: Was not presented in the book.
};


// TODO: Texture not implemented, faceIndices was not in the book.
std::vector<std::shared_ptr<Shape>> CreateTriangleMesh(const Transform *ObjectToWorld, const Transform *WorldToObject, bool reverseOrientation,
                                                       i32 nTriangles, const i32 *vertexIndices,
                                                       i32 nVertices, const Point3_t *positions,
                                                       const Vector3_t *tangents, const Normal3_t *normals, const Point2_t *uv);


PBR_NAMESPACE_END
