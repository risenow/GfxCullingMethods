#include "randomutils.h"
#include <random>

float Random01()
{
    static const size_t res = 10000;
    return float(rand()) / float(RAND_MAX);
}
float RandomFromTo(float from, float to)
{
    float t1 = Random01();
    float t2 = (to - from);
    float t3 = t1 * t2;
    float t4 = t3 + from;

    return Random01() * (to - from) + from;
}
glm::vec3 RandomFromTo3(float from, float to)
{
    return glm::vec3(RandomFromTo(from, to), RandomFromTo(from, to), RandomFromTo(from, to));
}