#include "LoPoApproxGeom.h"

//LoPoTriangle

LoPoTriangle::LoPoTriangle() {}
LoPoTriangle::LoPoTriangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
{
    m_Verts[0] = v1;
    m_Verts[1] = v2;
    m_Verts[2] = v3;
}

bool LoPoTriangle::IntersectSeg(const glm::vec3& origin, const glm::vec3& dir)
{
    glm::vec2 p;
    float dist = 0.0f;
    bool intersect = glm::intersectRayTriangle(origin, dir, m_Verts[0], m_Verts[1], m_Verts[2], p, dist);
    float len = glm::length(dir);
    dist *= len;

    return intersect && (dist <= len && dist >= 0.0f);
}

glm::vec3& LoPoTriangle::operator[](size_t i)
{
    assert(i < 3);
    return m_Verts[i];
}


//LoPoApproxGeom

LoPoApproxGeom::LoPoApproxGeom()
{}
LoPoApproxGeom::LoPoApproxGeom(const std::initializer_list<LoPoTriangle>& initList) : m_Prims(initList) {}
LoPoApproxGeom::LoPoApproxGeom(const std::vector < LoPoTriangle>& prims) : m_Prims(prims) {}

bool LoPoApproxGeom::IntersectSeg(const glm::vec3& origin, const glm::vec3& dir)
{
    for (LoPoTriangle& tr : m_Prims)
        if (tr.IntersectSeg(origin, dir))
            return true;
    return false;
}

AABB LoPoApproxGeom::CalcAABB()
{
    AABB aabb;
    for (LoPoTriangle& tr : m_Prims)
        for (size_t i = 0; i < 3; i++)
            aabb.Extend(tr[i]);
    return aabb;
}

void LoPoApproxGeom::GenXYAlignedQuad(LoPoApproxGeom& geom, const glm::vec3& topLeft, float width, float height)
{
    geom.m_Prims.push_back(LoPoTriangle(topLeft,
        glm::vec3(topLeft.x + width, topLeft.y, topLeft.z),
        glm::vec3(topLeft.x + width, topLeft.y - height, topLeft.z)));
    geom.m_Prims.push_back(LoPoTriangle(glm::vec3(topLeft.x + width, topLeft.y - height, topLeft.z),
        glm::vec3(topLeft.x, topLeft.y - height, topLeft.z),
        topLeft));
}