#pragma once
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtx/intersect.hpp"
#include "AABB.h"

class LoPoApproxGeom;
class LoPoTriangle
{
public:
    friend class LoPoApproxGeom;

    LoPoTriangle();
    LoPoTriangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);

    bool IntersectSeg(const glm::vec3& origin, const glm::vec3& dir);

    glm::vec3& operator[](size_t i);
private:
    glm::vec3 m_Verts[3];
};

class LoPoApproxGeom
{
public:
    LoPoApproxGeom();
    LoPoApproxGeom(const std::initializer_list<LoPoTriangle>& initList);
    LoPoApproxGeom(const std::vector < LoPoTriangle>& prims);

    bool IntersectSeg(const glm::vec3& origin, const glm::vec3& dir);

    AABB CalcAABB();

    static void GenXYAlignedQuad(LoPoApproxGeom& geom, const glm::vec3& topLeft, float width, float height);
private:
    std::vector<LoPoTriangle> m_Prims;
};