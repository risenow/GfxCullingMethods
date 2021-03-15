#pragma once
#include <algorithm>
#include <limits>
#include "mathutils.h"
#include "glm/glm.hpp"

struct AABB
{
    AABB() : m_Min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()),
             m_Max(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()),
             m_Centroid((m_Min + m_Max)*0.5f){}
    AABB(const glm::vec3& min, const glm::vec3& max) : m_Min(min), m_Max(max)
    {
        m_Centroid = (m_Min + m_Max) * 0.5f;
    }
    float Side()
    {
        return m_Max.x - m_Min.x;
    }
    enum
    {
        RightMask = 1,
        TopMask = 1 << 1,
        FrontMask = 1 << 2
    };
    enum
    {
        LeftBottomBack = 0,
        RightBottomBack = RightMask,
        LeftTopBack = TopMask,
        LeftBottomFront = FrontMask,
        RightTopBack = RightMask | TopMask,
        RightBottomFront = RightMask | FrontMask,
        LeftTopFront = TopMask | FrontMask,
        RightTopFront = RightMask | TopMask | FrontMask
    };
    glm::vec3 GetPoint(int i) const
    {
        const glm::vec3 metaOffset = m_Max - m_Min;
        /*const glm::vec3 offsets[8] = {glm::vec3(),
                                      glm::vec3(metaOffset.x, 0.0f, 0.0f),
                                      glm::vec3(0.0f, metaOffset.y, 0.0f),
                                      glm::vec3(0.0f, 0.0f, metaOffset.z), 
                                      glm::vec3(metaOffset.x, metaOffset.y, 0.0f),
                                      glm::vec3(metaOffset.x, 0.0f, metaOffset.z),
                                      glm::vec3(0.0f, metaOffset.y, metaOffset.z), 
                                      glm::vec3(metaOffset.x, metaOffset.y, metaOffset.z) };*/
        //
        float cx = (i & RightMask);
        float cy = ((i & TopMask) >> 1);
        float cz = ((i & FrontMask) >> 2);
        return glm::vec3(m_Min.x + metaOffset.x * cx, m_Min.y + metaOffset.y * cy, m_Min.z + metaOffset.z * cz);
    }

    bool IsValid() const
    {
        return m_Max.x >= m_Min.x && m_Max.y >= m_Min.y && m_Max.z >= m_Min.z;
    }

    float Area()
    {
        if (!IsValid())
            return 0.0f;
        return ((m_Max.x - m_Min.x) * (m_Max.y - m_Min.y) + (m_Max.x - m_Min.x) * (m_Max.z - m_Min.z) + (m_Max.y - m_Min.y) * (m_Max.z - m_Min.z)) * 2.0f;
    }

    bool Contains(const AABB& other) const
    {
        if (!other.IsValid())
            return false;
        return (m_Max.x >= other.m_Max.x && m_Max.y >= other.m_Max.y && m_Max.z >= other.m_Max.z &&
            m_Min.x <= other.m_Min.x && m_Min.y <= other.m_Min.y && m_Min.z <= other.m_Min.z);
    }
    bool Contains_(const AABB& other) const //without validness check
    {
        return (m_Max.x >= other.m_Max.x && m_Max.y >= other.m_Max.y && m_Max.z >= other.m_Max.z &&
            m_Min.x <= other.m_Min.x && m_Min.y <= other.m_Min.y && m_Min.z <= other.m_Min.z);//(glm::all(glm::greaterThanEqual(m_Max, other.m_Max)) && glm::all(glm::lessThanEqual(m_Min, other.m_Min)));
    }

    bool Contains(const glm::vec3& p) const
    {
        return (p.x >= m_Min.x && p.x <= m_Max.x && p.y >= m_Min.y && p.y <= m_Max.y && p.z >= m_Min.z && p.z <= m_Max.z);
    }

    bool VolumeIntersect(const AABB& other)
    {
        for (size_t i = 0; i < 8; i++)
            if (Contains(other.GetPoint(i)))
                return true;

        if (other.Contains(*this))
            return true;
        return false;
    }
    bool VolumeIntersect_(const AABB& other) //without additional checks
    {
        for (size_t i = 0; i < 8; i++)
            if (Contains(other.GetPoint(i)))
                return true;

        if (other.Contains_(*this))
            return true;
        return false;
    }

    union
    {
        struct {
            glm::vec3 m_Min;
            glm::vec3 m_Max;
        };
        glm::vec3 m_MinMax[2];
    };

    glm::vec3 m_Centroid;

    void ExtendMin(const glm::vec3& v)
    {
        m_Min = glm::vec3(std::min(m_Min.x, v.x), std::min(m_Min.y, v.y), std::min(m_Min.z, v.z));;
    }
    void ExtendMax(const glm::vec3& v)
    {
        m_Max = glm::vec3(std::max(m_Max.x, v.x), std::max(m_Max.y, v.y), std::max(m_Max.z, v.z));
    }
    void Extend(const glm::vec3& v)
    {
        ExtendMin(v);
        ExtendMax(v);
        m_Centroid = (m_Min + m_Max) * 0.5f;
    }
    
    void Extend(const AABB& v)
    {
        ExtendMin(v.m_Min);
        ExtendMax(v.m_Max);
        //Extend(v.m_Min);
        //Extend(v.m_Max);
        m_Centroid = (m_Min + m_Max) * 0.5f;
    }

    void ConvertToCube()
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
};