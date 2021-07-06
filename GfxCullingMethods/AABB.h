#pragma once
#include <algorithm>
#include <limits>
#include "mathutils.h"
#include "glm/glm.hpp"

struct AABB
{
    AABB();
    AABB(const glm::vec3& min, const glm::vec3& max);
    AABB(const glm::vec4& sbb);
    float Side();
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
    glm::vec4 GetSBB();
    glm::vec3 GetPoint(int i) const;

    bool IsValid() const;

    float Area();

    bool Contains(const AABB& other) const;
    bool Contains_(const AABB& other) const; //without validness check

    bool Contains(const glm::vec3& p) const;

    bool VolumeIntersect(const AABB& other);
    bool VolumeIntersect_(const AABB& other); //without additional checks

    union
    {
        struct {
            glm::vec3 m_Min;
            glm::vec3 m_Max;
        };
        glm::vec3 m_MinMax[2];
    };

    glm::vec3 m_Centroid;

    void ExtendMin(const glm::vec3& v);
    void ExtendMax(const glm::vec3& v);
    void Extend(const glm::vec3& v);
    
    void Extend(const AABB& v);
    void ConvertToCube();
};