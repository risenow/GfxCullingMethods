#include "mathutils.h"

bool FEQUAL(float v1, float v2, float eps)
{
    return (v1 >= v2 - eps && v1 <= v2 + eps);
}

glm::vec3 ProjectVecOnPlane(const glm::vec3& vec, const glm::vec3& n)
{
    return vec - glm::dot(vec, n) * n;
}

//G-d's help is appreciated here
bool PlaneIntersectsPositiveSubspaces(const Plane& p, const Plane& p1, const Plane& p2)
{
    const float A3 = p.n.x;
    const float B3 = p.n.y;
    const float C3 = p.n.z;
    const float D3 = p.d;

    const float A1 = p1.n.x;
    const float B1 = p1.n.y;
    const float C1 = p1.n.z;
    const float D1 = p1.d;

    const float A2 = p2.n.x;
    const float B2 = p2.n.y;
    const float C2 = p2.n.z;
    const float D2 = p2.d;

    float K1 = 0.0f;
    float K2 = 0.0f;

    if (!FEQUAL(B3, 0.0f))
    {
        K1 = (A1 * B3 - B1 * A3) / B3;
        K2 = (A2 * B3 - B2 * A3) / B3;

        if (!FEQUAL(K1, .0f) && !FEQUAL(K2, .0f))
        {
            if (K1 * K2 > 0.0f)
                return true;

            float k1 = (B1 * C3 - C1 * B3) / (A1 * B3 - B1 * A3);
            float k2 = (B2 * C3 - C2 * B3) / (A2 * B3 - B2 * A3);

            float b1 = (B1 * D3 - D1 * B3) / (A1 * B3 - B1 * A3);
            float b2 = (B2 * D3 - D2 * B3) / (A2 * B3 - B2 * A3);

            if (K1 > 0.0f && K2 < 0.0f)
            {
                return !(FEQUAL(k1, k2) && b2 < b1); // test b2 < b1
            }
            if (K1 < 0.0f && K2 > 0.0f)
            {
                return !(FEQUAL(k1, k2) && b1 < b2); // test 1 < b2
            }
        }
        if (FEQUAL(K1, 0.0f) && !FEQUAL(K2, 0.0f))
        {
            float K1p = (B1 * C3 - C1 * B3) / B3;
            if (FEQUAL(K1p, 0.0f))
            {
                return 0 > (B1 * D3 - D1 * B3) / B3;
            }
            else
                return true;
        }
        if (FEQUAL(K2, 0.0f) && !FEQUAL(K1, 0.0f))
        {
            float K2p = (B2 * C3 - C2 * B3) / B3;
            if (FEQUAL(K2p, 0.0f))
            {
                return 0 > (B2 * D3 - D2 * B3) / B3;
            }
            else
                return true;
        }
        if (FEQUAL(K1, 0.0f) && FEQUAL(K2, 0.0f))
        {
            float K1p = (B1 * C3 - C1 * B3) / B3;
            float K2p = (B2 * C3 - C2 * B3) / B3;

            float t1 = (B1 * D3 - D1 * B3) / (C1 * B3 - B1 * C3);
            float t2 = (B2 * D3 - D2 * B3) / (C2 * B3 - B2 * C3);

            if (K1p * K2p > 0)
                return true;
            if (K1p > 0.0f && K2p < 0.0f)
                return t1 < t2;
            if (K2p > 0.0f && K1p < 0.0f)
                return t2 < t1;
            bool K1pZr = FEQUAL(K1p, 0.0f);
            bool K2pZr = FEQUAL(K2p, 0.0f);
            bool ineq1 = 0 > (B1 * D3 - D1 * B3) / B3;
            bool ineq2 = 0 > (B2 * D3 - D2 * B3) / B3;
            if (K1pZr && !K2pZr)
                return ineq1;
            if (!K1pZr && K2pZr)
                return ineq2;
            return ineq1 && ineq2;
        }
    }
}

glm::bvec3 less3b(const glm::vec3& v1, const glm::vec3& v2)
{
    return glm::bvec3(v1.x < v2.x, v1.y < v2.y, v1.z < v2.z);
}