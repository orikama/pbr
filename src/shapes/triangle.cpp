#include "triangle.h"
#include "../core/stats.h"
#include "../core/efloat.hpp"


// TODO: CreateTriangleMeshShape(), Triangle::SolidAngle(), Triangle::Sample() are not implemented. Intersect methods are not finished.


PBR_NAMESPACE_BEGIN

// ---------------------------------------
// ---------- UTILITY FUNCTIONS ----------
// ---------------------------------------

// NOTE: And once again, I'm sure that all this std::vector and std::shared_ptr can be simplified to std::array and std::unique_ptr.
//       May be it would be even better to use std::unique_ptr<std::array>> than std::array<std::unique_ptr>>, or something like that.
std::vector<std::shared_ptr<Shape>> CreateTriangleMesh(const Transform *ObjectToWorld, const Transform *WorldToObject, bool reverseOrientation,
                                                       i32 nTriangles, const i32 *vertexIndices,
                                                       i32 nVertices, const Point3_t *positions,
                                                       const Vector3_t *tangents, const Normal3_t *normals, const Point2_t *uv)
{
    auto mesh = std::make_shared<TriangleMesh>(*ObjectToWorld, nTriangles, vertexIndices, nVertices, positions, tangents, normals, uv);
    
    std::vector<std::shared_ptr<Shape>> triangles;
    triangles.reserve(nTriangles);
    for(int i; i < nTriangles; ++i)
        // FINDOUT: emplace_back ?
        triangles.push_back(std::make_shared<Triangle>(ObjectToWorld, WorldToObject, reverseOrientation, mesh, i));
    
    return triangles;
}


// ******************************************************************************
// -------------------------------- TriangleMesh --------------------------------
// ******************************************************************************

// ---------------------------------------
// ------------ CONSTRUCTORS -------------
// ---------------------------------------

PBR_STATS_MEMORY_COUNTER("Memory/Triangle meshes", stats_triangleMeshBytes)
PBR_STATS_RATIO("Scene/Triangles per triangle mesh", stats_nTriangles, stats_nMeshes)

TriangleMesh::TriangleMesh(const Transform &ObjectToWorld,
                           i32 _nTriangles, const i32 *_vertexIndices,
                           i32 _nVertices, const Point3_t *_positions,
                           const Vector3_t *_tangents, const Normal3_t *_normals, const Point2_t *_uv
                           //const std::shared_ptr<Texture<fp_t>> &alphaMask,
                           //const std::shared_ptr<Texture<fp_t>> &shadingMask,
                           /*const i32 *_faceIndices*/)
    : nTriangles(_nTriangles)
    , nVertices(_nVertices)
    , vertexIndices(_vertexIndices, _vertexIndices + 3 * _nTriangles)
{
    PBR_STATS_VARIABLE_INCREMENT(stats_nMeshes)
    PBR_STATS_VARIABLE_ADD(stats_nTriangles, nTriangles)
    PBR_STATS_VARIABLE_ADD(stats_triangleMeshBytes, sizeof(*this) + vertexIndices.size() * sizeof(i32) + nVertices * sizeof(*_positions))

    // DIFFERENCE: Why they are using unique_ptr.reset() ?
    positions = std::make_unique<Point3_t[]>(nVertices);
    for(int i = 0; i < nVertices; ++i)
        positions[i] = ObjectToWorld(_positions[i]);
    
    if(_uv != nullptr) {
        PBR_STATS_VARIABLE_ADD(stats_triangleMeshBytes, nVertices * sizeof(*_uv))

        uv = std::make_unique<Point2_t[]>(nVertices);
        std::copy(_uv, _uv + nVertices, uv.get()); // FINDOUT: Will it be as effective as memcpy() ?
    }
    if(_normals != nullptr) {
        PBR_STATS_VARIABLE_ADD(stats_triangleMeshBytes, nVertices * sizeof(*_normals))

        normals = std::make_unique<Normal3_t[]>(nVertices);
        for(int i = 0; i < nVertices; ++i)
            normals[i] = ObjectToWorld(_normals[i]);
    }
    if(_tangents != nullptr) {
        PBR_STATS_VARIABLE_ADD(stats_triangleMeshBytes, nVertices * sizeof(*_tangents))

        tangents = std::make_unique<Vector3_t[]>(nVertices);
        for(int i = 0; i < nVertices; ++i)
            tangents[i] = ObjectToWorld(_tangents[i]);
    }
}


// ******************************************************************************
// ---------------------------------- Triangle ----------------------------------
// ******************************************************************************

// ---------------------------------------
// ------------ CONSTRUCTORS -------------
// ---------------------------------------

Triangle::Triangle(const Transform *ObjectToWorld, const Transform *WorldToObject, bool reverseOrientation,
                   const std::shared_ptr<TriangleMesh> &mesh, i32 triangleIndex)
    : Shape(ObjectToWorld, WorldToObject, reverseOrientation)
    , m_mesh(mesh)
    // NOTE: Convert a shared_ptr to vector, to just a pointer to i32, isn't it a great move? Obviously it works(at least if vector will not be resized), but what the fuck.
    // TODO: Possible use of std::span, but will require more memory.
    , m_vIndices(&mesh->vertexIndices[3 * triangleIndex])
{
    PBR_STATS_VARIABLE_ADD(stats_triangleMeshBytes, sizeof(*this))
}


// ---------------------------------------
// --------------- METHODS ---------------
// ---------------------------------------

Bounds3_t Triangle::ObjectBound() const
{
    return Union(Bounds3_t((*WorldToObject)(m_mesh->positions[m_vIndices[0]]),
                           (*WorldToObject)(m_mesh->positions[m_vIndices[1]])),
                 (*WorldToObject)(m_mesh->positions[m_vIndices[2]])
                );
}

Bounds3_t Triangle::WorldBound() const
{
    return Union(Bounds3_t(m_mesh->positions[m_vIndices[0]],
                           m_mesh->positions[m_vIndices[1]]),
                 m_mesh->positions[m_vIndices[2]]
                );
}

void Triangle::GetUV(Point2_t out_uv[3]) const
{
    if (m_mesh->uv != nullptr) {
        out_uv[0] = m_mesh->uv[m_vIndices[0]];
        out_uv[1] = m_mesh->uv[m_vIndices[1]];
        out_uv[2] = m_mesh->uv[m_vIndices[2]];
    }
    else {
        out_uv[0] = Point2_t(0, 0);
        out_uv[1] = Point2_t(1, 0);
        out_uv[2] = Point2_t(1, 1);
    }
}

PBR_STATS_PERCENT("Intersections/Ray-triangle intersection tests", stats_nHits, stats_nTests)
// TODO: Texture not implemented, and therefore test against alpha texture intersection.
// TODO: out_isect is not filled
// Watertight Ray/Triangle intersection
bool Triangle::Intersect(const Ray_arg r,
                         fp_t &out_tHit, SurfaceInteraction &out_isect,
                         bool testAlphaTexture = true) const
{
    PBR_PROFILE_FUNCTION(ProfileCategory::Triangle_Intersect)
    PBR_STATS_VARIABLE_INCREMENT(stats_nTests)

    // Get triangle vertices
    // DIFFERENCE: I'm pretty sure that copy is better than const reference.
    Point3_t p0 = m_mesh->positions[m_vIndices[0]];
    Point3_t p1 = m_mesh->positions[m_vIndices[1]];
    Point3_t p2 = m_mesh->positions[m_vIndices[2]];

    // Transform triangle vertices to ray coordinate space(relative to ray origin)
    Point3_t p0t = p0 - Vector3_t(r.origin);
    Point3_t p1t = p1 - Vector3_t(r.origin);
    Point3_t p2t = p2 - Vector3_t(r.origin);
    // Calculate dimension where the ray direction is maximal
    i32 kz = MaxDimension(Abs(r.direction));
    i32 kx = kz + 1; if (kx == 3) kx = 0;
    i32 ky = kx + 1; if (ky == 3) ky = 0;
    // Permute components of triangle vertices and ray direction
    Vector3_t direction = Permute(r.direction, kx, ky, kz);
    p0t = Permute(p0t, kx, ky, kz);
    p1t = Permute(p1t, kx, ky, kz);
    p2t = Permute(p2t, kx, ky, kz);
    // Apply shear transformation to translated vertex positions
    // FINDOUT: Compute Sz first and then Sx=-direction.x * Sz ?
    fp_t Sx = -direction.x / direction.z;
    fp_t Sy = -direction.y / direction.z;
    fp_t Sz = fp_t(1) / direction.z; // FINDOUT: Move where it's used ? Although probably compiler will do it anyway.
    p0t.x += Sx * p0t.z;
    p0t.y += Sy * p0t.z;
    p1t.x += Sx * p1t.z;
    p1t.y += Sy * p1t.z;
    p2t.x += Sx * p2t.z;
    p2t.y += Sy * p2t.z;

    // Compute edge function coefficients
    fp_t e0 = p1t.x * p2t.y - p1t.y * p2t.x;
    fp_t e1 = p2t.x * p0t.y - p2t.y * p0t.x;
    fp_t e2 = p0t.x * p1t.y - p0t.y * p1t.x;
    // Fall back to double precision test at triangle edges
    if (sizeof(fp_t) == sizeof(f32) && (e0 == 0 || e1 == 0 || e2 == 0)) {
        e0 = f32(f64(p1t.x) * f64(p2t.y) - f64(p1t.y) * f64(p2t.x));
        e1 = f32(f64(p2t.x) * f64(p0t.y) - f64(p2t.y) * f64(p0t.x));
        e2 = f32(f64(p0t.x) * f64(p1t.y) - f64(p0t.y) * f64(p1t.x));
    }
    // Perform triangle edge and determinant tests
    if ((e0 < 0 || e1 < 0 || e2 < 0) && (e0 >0 || e1 > 0 || e2 > 0))
        return false;
    fp_t det = e0 + e1 + e2;
    if (det == 0)
        return false;

    // Compute scaled hit distance to triangle and test against ray $tMax$ range
    p0t.z *= Sz;
    p1t.z *= Sz;
    p2t.z *= Sz;
    fp_t tScaled = e0 * p0t.z + e1 * p1t.z + e2 * p2t.z;
    // NOTE: Probably can be simplified
    if (det < 0 && (tScaled >= 0 || tScaled < r.tMax * det))
        return false;
    else if (det > 0 && (tScaled <= 0 || tScaled > r.tMax * det))
        return false;
    // Compute barycentric coordinates and triangle intersection
    fp_t rcpDet = fp_t(1) / det;
    fp_t b0 = e0 * rcpDet;
    fp_t b1 = e1 * rcpDet;
    fp_t b2 = e2 * rcpDet;
    fp_t t = tScaled * rcpDet;

//#if PBR_PBR_ENABLE_EFLOAT == 1
    fp_t maxXt = MaxComponent(Abs(Vector3_t(p0t.x, p1t.x, p2t.x)));
    fp_t maxYt = MaxComponent(Abs(Vector3_t(p0t.y, p1t.y, p2t.y)));
    fp_t maxZt = MaxComponent(Abs(Vector3_t(p0t.z, p1t.z, p2t.z)));
    fp_t deltaX = Gamma(5) * (maxXt + maxZt);
    fp_t deltaY = Gamma(5) * (maxYt + maxZt);
    fp_t deltaZ = Gamma(3) * maxZt;
    
    fp_t deltaE = fp_t(2) * (Gamma(2) * maxXt * maxYt + deltaY * maxXt + deltaX * maxYt);
    fp_t maxE = MaxComponent(Abs(Vector3_t(e0, e1, e2)));
    fp_t deltaT = fp_t(3) * (Gamma(3) * maxE * maxZt + deltaE * maxZt + deltaZ * maxE) * std::abs(rcpDet);
    if (t <= deltaT)
        return false;
//#endif

    // Compute triangle partial derivatives
    Point2_t uv[3];
    GetUV(uv);
    // Compute deltas for triangle partial derivatives
    Vector2_t duv02 = uv[0] - uv[2];
    Vector2_t duv12 = uv[1] - uv[2];
    Vector3_t dp02 = p0 - p2;
    Vector3_t dp12 = p1 - p2;
    fp_t determinant = duv02.x * duv12.y - duv02.y * duv12.x;

    Vector3_t dpdu, dpdv;
    bool degenerateUV = std::abs(determinant) < fp_t(1e-8);
    if (degenerateUV == false) {
        fp_t rcpDeterminant = fp_t(1) / determinant;
        dpdu = (duv12.y * dp02 - duv02.y * dp12) * rcpDeterminant;
        dpdv = (-duv12.x * dp02 + duv02.x * dp12) * rcpDeterminant;
    }
    // DIFFERENCE: Update from github, different from the book, not sure how it works :(
    if (degenerateUV || Cross(dpdu, dpdv).LengthSquared() == 0) {
        // Handle zero determinant for triangle partial derivative matrix
        Vector3_t ng = Cross(p2 - p0, p1 - p0);
        // FINDOUT: Why the fuck this should happen by the time we got this far ?
        if (ng.LengthSquared() == 0)
            // The triangle is actually degenerate; the intersection is bogus
            return false;
        
        CoordinateSystem(Normalize(ng), dpdu, dpdv);
    }

    // NOTE: And yet another fucking error bounds in the same function.
    // FINDOUT: I'm that it would be faster just to use f64 instead of f32 with all this fuckin bounds.
    //          Although I still don't know if there is a way to get rid of pError computation.
//#if PBR_PBR_ENABLE_EFLOAT == 1
    fp_t xAbsSum = std::abs(b0 * p0.x) + std::abs(b1 * p1.x) + std::abs(b2 * p2.x);
    fp_t yAbsSum = std::abs(b0 * p0.y) + std::abs(b1 * p1.y) + std::abs(b2 * p2.y);
    fp_t zAbsSum = std::abs(b0 * p0.z) + std::abs(b1 * p1.z) + std::abs(b2 * p2.z);
    Vector3_t pError = Gamma(7) * Vector3_t(xAbsSum, yAbsSum, zAbsSum);
//#endif


    out_tHit = t;

    PBR_STATS_VARIABLE_INCREMENT(stats_nHits)

    return true;
}

// TODO: Test shadow ray intersection against alpha texture
// Watertight Ray/Triangle intersection
bool Triangle::IsIntersecting(const Ray_arg r, bool testAlphaTexture = true) const
{
    PBR_PROFILE_FUNCTION(ProfileCategory::Triangle_IsIntersecting)
    PBR_STATS_VARIABLE_INCREMENT(stats_nTests)

    // Get triangle vertices
    // DIFFERENCE: I'm pretty sure that copy is better than const reference.
    Point3_t p0 = m_mesh->positions[m_vIndices[0]];
    Point3_t p1 = m_mesh->positions[m_vIndices[1]];
    Point3_t p2 = m_mesh->positions[m_vIndices[2]];

    // Transform triangle vertices to ray coordinate space(relative to ray origin)
    Point3_t p0t = p0 - Vector3_t(r.origin);
    Point3_t p1t = p1 - Vector3_t(r.origin);
    Point3_t p2t = p2 - Vector3_t(r.origin);
    // Calculate dimension where the ray direction is maximal
    i32 kz = MaxDimension(Abs(r.direction));
    i32 kx = kz + 1; if (kx == 3) kx = 0;
    i32 ky = kx + 1; if (ky == 3) ky = 0;
    // Permute components of triangle vertices and ray direction
    Vector3_t direction = Permute(r.direction, kx, ky, kz);
    p0t = Permute(p0t, kx, ky, kz);
    p1t = Permute(p1t, kx, ky, kz);
    p2t = Permute(p2t, kx, ky, kz);
    // Apply shear transformation to translated vertex positions
    // FINDOUT: Compute Sz first and then Sx=-direction.x * Sz ?
    fp_t Sx = -direction.x / direction.z;
    fp_t Sy = -direction.y / direction.z;
    fp_t Sz = fp_t(1) / direction.z; // FINDOUT: Move where it's used ? Although probably compiler will do it anyway.
    p0t.x += Sx * p0t.z;
    p0t.y += Sy * p0t.z;
    p1t.x += Sx * p1t.z;
    p1t.y += Sy * p1t.z;
    p2t.x += Sx * p2t.z;
    p2t.y += Sy * p2t.z;

    // Compute edge function coefficients
    fp_t e0 = p1t.x * p2t.y - p1t.y * p2t.x;
    fp_t e1 = p2t.x * p0t.y - p2t.y * p0t.x;
    fp_t e2 = p0t.x * p1t.y - p0t.y * p1t.x;
    // Fall back to double precision test at triangle edges
    if (sizeof(fp_t) == sizeof(f32) && (e0 == 0 || e1 == 0 || e2 == 0)) {
        e0 = f32(f64(p1t.x) * f64(p2t.y) - f64(p1t.y) * f64(p2t.x));
        e1 = f32(f64(p2t.x) * f64(p0t.y) - f64(p2t.y) * f64(p0t.x));
        e2 = f32(f64(p0t.x) * f64(p1t.y) - f64(p0t.y) * f64(p1t.x));
    }
    // Perform triangle edge and determinant tests
    if ((e0 < 0 || e1 < 0 || e2 < 0) && (e0 >0 || e1 > 0 || e2 > 0))
        return false;
    fp_t det = e0 + e1 + e2;
    if (det == 0)
        return false;

    // Compute scaled hit distance to triangle and test against ray $tMax$ range
    p0t.z *= Sz;
    p1t.z *= Sz;
    p2t.z *= Sz;
    fp_t tScaled = e0 * p0t.z + e1 * p1t.z + e2 * p2t.z;
    // NOTE: Probably can be simplified
    if (det < 0 && (tScaled >= 0 || tScaled < r.tMax * det))
        return false;
    else if (det > 0 && (tScaled <= 0 || tScaled > r.tMax * det))
        return false;
    // Compute barycentric coordinates and triangle intersection
    fp_t rcpDet = fp_t(1) / det;
    fp_t b0 = e0 * rcpDet;
    fp_t b1 = e1 * rcpDet;
    fp_t b2 = e2 * rcpDet;
    fp_t t = tScaled * rcpDet;

//#if PBR_PBR_ENABLE_EFLOAT == 1
    fp_t maxXt = MaxComponent(Abs(Vector3_t(p0t.x, p1t.x, p2t.x)));
    fp_t maxYt = MaxComponent(Abs(Vector3_t(p0t.y, p1t.y, p2t.y)));
    fp_t maxZt = MaxComponent(Abs(Vector3_t(p0t.z, p1t.z, p2t.z)));
    fp_t deltaX = Gamma(5) * (maxXt + maxZt);
    fp_t deltaY = Gamma(5) * (maxYt + maxZt);
    fp_t deltaZ = Gamma(3) * maxZt;
    
    fp_t deltaE = fp_t(2) * (Gamma(2) * maxXt * maxYt + deltaY * maxXt + deltaX * maxYt);
    fp_t maxE = MaxComponent(Abs(Vector3_t(e0, e1, e2)));
    fp_t deltaT = fp_t(3) * (Gamma(3) * maxE * maxZt + deltaE * maxZt + deltaZ * maxE) * std::abs(rcpDet);
    if (t <= deltaT)
        return false;
//#endif



    PBR_STATS_VARIABLE_INCREMENT(stats_nHits)

    return true;
}

fp_t Triangle::Area() const
{
    // DIFFERENCE: Copy or reference ?
    // NOTE: _mm_prefetch for p2 ? Pog
    Point3_t p0 = m_mesh->positions[m_vIndices[0]];
    Point3_t p1 = m_mesh->positions[m_vIndices[1]];
    Point3_t p2 = m_mesh->positions[m_vIndices[2]];
    // NOTE: Division by 2 should be optimized to multiplication by 0.5;
    return Cross(p1 - p0, p2 - p0).Length() / fp_t(2);
}

PBR_NAMESPACE_END
