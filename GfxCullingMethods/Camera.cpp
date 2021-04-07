#include "Camera.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/matrix_interpolation.hpp"

#include <iostream>

Camera::Camera(const glm::vec3& position, const glm::vec3& rotation) : m_Position(position), m_Rotation(rotation), m_UseAngles(true)
{
    UpdateViewProjectionMatrix();
}

glm::vec3 GetRow3D(const glm::mat4x4& m, size_t i)
{
    return glm::vec3(m[0][i], m[1][i], m[2][i]);
}

glm::mat4x4 ViewIdentity()
{
    glm::mat4x4 m = glm::identity<glm::mat4x4>();
    return m;
}

void Camera::SetOrientation(const glm::vec3& _orientation)
{
    glm::vec3 orientation = glm::normalize(_orientation);
    glm::vec3 viewVec = glm::vec3(0.0, 0.0, 1.0);

    glm::vec3 eyeXZ = orientation;
    eyeXZ.y = 0;
    eyeXZ = glm::normalize(eyeXZ);
    float signZY = glm::sign(glm::cross(eyeXZ, glm::vec3(viewVec.x, 0.0, viewVec.z)).y);

    float xzAngleCos = glm::clamp(glm::dot(eyeXZ, orientation), -1.0f, 1.0f); //wtf, normalized vector dot normalized vector gives slightly > 1.0
    float angleXZ = (glm::sign(orientation.y) * acos(xzAngleCos));
    float angleZY = signZY * acos(glm::dot(eyeXZ, glm::vec3(viewVec.x, 0.0f, viewVec.z)));

    std::cout << angleXZ << " " << angleZY << std::endl;

    m_Rotation.x = angleXZ;
    m_Rotation.y = -angleZY;

    m_Eye = orientation;
}

void Camera::UpdateViewProjectionMatrix()
{
    static const glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
    static const glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    static const glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);

    m_ViewMatrix = ViewIdentity();
   
    m_ViewVec = glm::vec3(0.0, 0.0, 1.0);

    if (m_UseAngles)
    {
        m_ViewMatrix = glm::rotate(m_ViewMatrix, m_Rotation.x, xAxis);
        m_ViewMatrix = glm::rotate(m_ViewMatrix, -m_Rotation.y, yAxis);
        m_ViewMatrix = (glm::translate(m_ViewMatrix, -m_Position));
        //m_ViewMatrix = (glm::translate(m_ViewMatrix, m_Position));
        //m_ViewMatrix = glm::inverse(m_ViewMatrix);
    }
    else
    {
        m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Eye, glm::vec3(0.0, 1.0, 0.0));
    }

    glm::mat4x4 temp = glm::transpose(m_ViewMatrix);
    m_LeftVec = temp[0];
    m_TopVec = temp[1];
    m_ViewVec = temp[2];

    glm::vec2 projFactors = GetProjectionFactors();

    glm::vec4 wP = glm::vec4(300.0, 400.0, 1.0, 1.0);
    glm::vec4 vP = m_ViewMatrix * wP;

    glm::vec4 restrWP = glm::inverse(m_ViewMatrix) * vP;

    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix; 
}

void Camera::StepForward(float step)
{
    m_Position += m_ViewVec * step;
}

void Camera::StepLeft(float step)
{
    m_Position -= m_LeftVec * step;
}

void Camera::SetPosition(const glm::vec3& position)
{
    m_Position = position;
    UpdateViewProjectionMatrix();
}
void Camera::SetRotation(const glm::vec3& rotation)
{
    m_Rotation = rotation;
    UpdateViewProjectionMatrix();
}

void Camera::SetPositionRotation(const glm::vec3& position, const glm::vec3& rotation)
{
    m_Position = position;
    m_Rotation = rotation;
    UpdateViewProjectionMatrix();
}

void Camera::SetProjection(float fov, float aspect, float _near, float _far) //  aspect == width/height
{
    m_Fov = fov;
    m_Aspect = aspect;
    m_Near = _near;
    m_Far = _far;

    m_ProjectionMatrix = (glm::perspectiveFov(glm::radians(m_Fov), aspect, 1.0f, m_Near, m_Far));

    UpdateViewProjectionMatrix();
}

void Camera::SetProjectionAsOrtho(float left, float right, float bottom, float top, float zNear, float zFar)
{
    m_ProjectionMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);
    UpdateViewProjectionMatrix();
}

void Camera::UpdateProjection(float aspect)
{
    m_Aspect = aspect;
    m_ProjectionMatrix = (glm::perspectiveFov(glm::radians(m_Fov), m_Aspect, 1.0f, m_Near, m_Far));
    UpdateViewProjectionMatrix();
}

const glm::vec3& Camera::GetRotation() const
{
    return m_Rotation;
}

const glm::vec3& Camera::GetPosition() const
{
    return m_Position;
}

const glm::mat4x4& Camera::GetViewMatrix() const
{
    return m_ViewMatrix;
}

const glm::mat4x4& Camera::GetViewProjectionMatrix() const
{
    return m_ViewProjectionMatrix;
}

const glm::mat4x4& Camera::GetProjectionMatrix() const
{
    return m_ProjectionMatrix;
}

glm::vec2 Camera::GetProjectionFactors() const
{
    return glm::vec2(m_Aspect * glm::tan(glm::radians(m_Fov / 2.0f)), glm::tan(glm::radians(m_Fov / 2.0f)));
}
