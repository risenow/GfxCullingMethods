#include "mathutils.h"

glm::bvec3 less3b(const glm::vec3& v1, const glm::vec3& v2)
{
    return glm::bvec3(v1.x < v2.x, v1.y < v2.y, v1.z < v2.z);
}