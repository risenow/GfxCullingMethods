#pragma once
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "mathutils.h"
#include "AABB.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <d3d11.h>

typedef glm::vec3 CameraPosition;
typedef glm::vec3 CameraRotation;

class OrientationBase
{
public:
    virtual glm::mat4x4 GetTransformation() = 0;
};

class OrientationEuler : public OrientationBase
{
public:
    void SetRotation(const glm::vec3& rot)
    {
        m_Rotation = rot;
    }
    virtual glm::mat4x4 GetTransformation() override
    {
        static const glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
        static const glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
        static const glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);
 
        glm::mat4x4 transform = glm::rotate(glm::identity<glm::mat4x4>(), m_Rotation.x, xAxis);
        transform = glm::rotate(transform, m_Rotation.y, yAxis);
    }
    glm::vec3 m_Rotation;
};

class Camera
{
public:
    Camera(const glm::vec3& position = glm::vec3(), const glm::vec3& rotation = glm::vec3());

    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::vec3& rotation);
    void SetOrientation(const glm::vec3& orientation);

    //calling UpdateViewProjectionMatrix once
    void SetPositionRotation(const glm::vec3& position, const glm::vec3& rotation);

    void SetProjection(float fov, float aspect, float near, float far);
    void SetProjectionAsOrtho(float left, float right, float bottom, float top, float zNear, float zFar);
    void UpdateProjection(float aspect);

    void StepForward(float step);
    void StepLeft(float step);

    const glm::mat4x4& GetViewProjectionMatrix() const;
    const glm::mat4x4& GetViewMatrix() const;
    const glm::mat4x4& GetProjectionMatrix() const;
    const glm::vec3& GetPosition() const;
    const glm::vec3& GetRotation() const;
    glm::vec2 GetProjectionFactors() const;

    void UpdateViewProjectionMatrix();

    struct Frustum
    {
        float CalcSolidAngle(const glm::mat4x4& m)
        {
            float tgAlpha = 1.0f / m[1][1];
            float tgBeta = 1.0f / m[0][0];
            float alpha = atan(tgAlpha);
            float beta = atan(tgBeta);

            return 4 * glm::asin(sin(alpha)*sin(beta));
        }
        Frustum() : solidAngle(0.0f) {}
        Frustum(const Camera& cam, bool inWorld = true) 
        {
            assert(inWorld);
            glm::mat4x4 m = cam.GetProjectionMatrix() * cam.GetViewMatrix();//glm::inverse(cam.GetViewMatrix());

            solidAngle = CalcSolidAngle(m);

            planes[0] = Plane(glm::vec3(m[0][3] + m[0][0], m[1][3] + m[1][0], m[2][3] + m[2][0]), m[3][3] + m[3][0]); //LEFT
            planes[1] = Plane(glm::vec3(m[0][3] - m[0][0], m[1][3] - m[1][0], m[2][3] - m[2][0]), m[3][3] - m[3][0]); //RIGHT
            planes[2] = Plane(glm::vec3(m[0][3] - m[0][1], m[1][3] - m[1][1], m[2][3] - m[2][1]), m[3][3] - m[3][1]); //top
            planes[3] = Plane(glm::vec3(m[0][3] + m[0][1], m[1][3] + m[1][1], m[2][3] + m[2][1]), m[3][3] + m[3][1]); //bottom
            planes[4] = Plane(glm::vec3(m[0][3] + m[0][2], m[1][3] + m[1][2], m[2][3] + m[2][2]), m[3][3] + m[3][2]); //NEAR
            planes[5] = Plane(glm::vec3(m[0][3] - m[0][2], m[1][3] - m[1][2], m[2][3] - m[2][2]), m[3][3] - m[3][2]); //FAR
        }

        

        bool Contains(const glm::vec3& p)
        {
            bool res = true;
            for (Plane& plane : planes)
                res = res && (plane.Test(p) > 0.0f);
            return res;
        }

        bool Test(const AABB& aabb)
        {
            //bool result = true;
            for (Plane& plane : planes)
            {
                int xId = plane.n.x > 0.0f;
                int yId = plane.n.y > 0.0f;
                int zId = plane.n.z > 0.0f;
                if (plane.Test(glm::vec3(aabb.m_MinMax[xId].x, aabb.m_MinMax[yId].y, aabb.m_MinMax[zId].z)) < 0.0f)
                    return false;
                //if (plane.Test(glm::vec3(aabb.m_MinMax[1 - xId].x, aabb.m_MinMax[1 - yId].y, aabb.m_MinMax[1 - zId].z)) <= 0.0f)
                //    return true;
            }
            return true;
        }
        float solidAngle;
        Plane planes[6];
    };

    glm::vec3 GetFrontVec()
    {
        return m_ViewVec;
    }
    glm::vec3 GetRightVec()
    {
        return m_LeftVec; // lol, fix it
    }
    glm::vec3 GetTopVec()
    {
        return m_TopVec;
    }
    //test
    glm::vec3 m_Eye;
    bool m_UseAngles;
private:

    glm::vec3 m_ViewVec;
    glm::vec3 m_LeftVec;
    glm::vec3 m_TopVec;
    glm::vec3 m_Position;
    glm::vec3 m_Rotation;

    glm::mat4x4 m_ProjectionMatrix;
    glm::mat4x4 m_ViewMatrix;
    glm::mat4x4 m_ViewProjectionMatrix;

    float m_Fov;
    float m_Near;
    float m_Far;
    float m_Aspect;
};