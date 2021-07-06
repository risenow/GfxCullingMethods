#include "AABB.h"

AABB::AABB() : m_Min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()),
    m_Max(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()),
    m_Centroid((m_Min + m_Max) * 0.5f)
{}
AABB::AABB(const glm::vec3& min, const glm::vec3& max) : m_Min(min), m_Max(max)
{
    m_Centroid = (m_Min + m_Max) * 0.5f;
}
AABB::AABB(const glm::vec4& sbb)
{
    m_Min = glm::vec3(sbb.x - sbb.w, sbb.y - sbb.w, sbb.z - sbb.w);
    m_Max = glm::vec3(sbb.x + sbb.w, sbb.y + sbb.w, sbb.z + sbb.w);
    m_Centroid = (m_Min + m_Max) * 0.5f;
}
float AABB::Side()
{
    return std::max(std::max(m_Max.x - m_Min.x, m_Max.y - m_Min.y), m_Max.z - m_Min.z); 
}

glm::vec4 AABB::GetSBB()
{
    glm::vec3 v = m_Max - m_Min;
    float maxSide = glm::length(v);
    return glm::vec4(m_Centroid.x, m_Centroid.y, m_Centroid.z, maxSide * 0.5f);//2.0f);
}
glm::vec3 AABB::GetPoint(int i) const
{
    int cx = (i & RightMask);
    int cy = ((i & TopMask) >> 1);
    int cz = ((i & FrontMask) >> 2);
    return glm::vec3(m_MinMax[cx].x, m_MinMax[cy].y, m_MinMax[cz].z);
}

bool AABB::IsValid() const
{
    return m_Max.x >= m_Min.x && m_Max.y >= m_Min.y && m_Max.z >= m_Min.z;
}

float AABB::Area()
{
    if (!IsValid())
        return 0.0f;
    return ((m_Max.x - m_Min.x) * (m_Max.y - m_Min.y) + (m_Max.x - m_Min.x) * (m_Max.z - m_Min.z) + (m_Max.y - m_Min.y) * (m_Max.z - m_Min.z)) * 2.0f;
}

bool AABB::Contains(const AABB& other) const
{
    if (!other.IsValid())
        return false;
    return (m_Max.x >= other.m_Max.x && m_Max.y >= other.m_Max.y && m_Max.z >= other.m_Max.z &&
        m_Min.x <= other.m_Min.x && m_Min.y <= other.m_Min.y && m_Min.z <= other.m_Min.z);
}
bool AABB::Contains_(const AABB& other) const //without validness check
{
    return (m_Max.x >= other.m_Max.x && m_Max.y >= other.m_Max.y && m_Max.z >= other.m_Max.z &&
        m_Min.x <= other.m_Min.x && m_Min.y <= other.m_Min.y && m_Min.z <= other.m_Min.z);//(glm::all(glm::greaterThanEqual(m_Max, other.m_Max)) && glm::all(glm::lessThanEqual(m_Min, other.m_Min)));
}

bool AABB::Contains(const glm::vec3& p) const
{
    return (p.x >= m_Min.x && p.x <= m_Max.x && p.y >= m_Min.y && p.y <= m_Max.y && p.z >= m_Min.z && p.z <= m_Max.z);
}

bool AABB::VolumeIntersect(const AABB& other)
{
    for (size_t i = 0; i < 8; i++)
        if (Contains(other.GetPoint(i)))
            return true;

    if (other.Contains(*this))
        return true;
    return false;
}
bool AABB::VolumeIntersect_(const AABB& other) //without additional checks
{
    for (size_t i = 0; i < 8; i++)
        if (Contains(other.GetPoint(i)))
            return true;

    if (other.Contains_(*this))
        return true;
    return false;
}

void AABB::ExtendMin(const glm::vec3& v)
{
    m_Min = glm::vec3(std::min(m_Min.x, v.x), std::min(m_Min.y, v.y), std::min(m_Min.z, v.z));;
}
void AABB::ExtendMax(const glm::vec3& v)
{
    m_Max = glm::vec3(std::max(m_Max.x, v.x), std::max(m_Max.y, v.y), std::max(m_Max.z, v.z));
}
void AABB::Extend(const glm::vec3& v)
{
    ExtendMin(v);
    ExtendMax(v);
    m_Centroid = (m_Min + m_Max) * 0.5f;
}

void AABB::Extend(const AABB& v)
{
    ExtendMin(v.m_Min);
    ExtendMax(v.m_Max);

    m_Centroid = (m_Min + m_Max) * 0.5f;
}

void AABB::ConvertToCube()
{
    float maxSide = -FLT_MAX;

    for (size_t i = 0; i < 3; i++)
        if (m_Max[i] - m_Min[i] > maxSide)
        {
            maxSide = m_Max[i] - m_Min[i];
        }

    for (size_t i = 0; i < 3; i++)
        m_Max[i] = m_Min[i] + maxSide;
}