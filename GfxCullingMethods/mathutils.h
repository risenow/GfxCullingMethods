#pragma once
#include "glm/glm.hpp"

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

glm::bvec3 less3b(const glm::vec3& v1, const glm::vec3& v2);