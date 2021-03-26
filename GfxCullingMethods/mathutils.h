#pragma once
#include "glm/glm.hpp"

bool FEQUAL(float v1, float v2, float eps = .0000001);

struct Plane
{
    Plane() : d(0.0f) {}
    Plane(const glm::vec3& _n, float _d) : n(_n), d(_d) {}

    float Test(const glm::vec3& p)
    {
        return glm::dot(p, n) + d;
    }

    glm::vec3 n;
    float d;
};

bool PlaneIntersectsPositiveSubspaces(const Plane& p, const Plane& p1, const Plane& p2);
glm::bvec3 less3b(const glm::vec3& v1, const glm::vec3& v2);

glm::vec3 ProjectVecOnPlane(const glm::vec3& vec, const glm::vec3& n);