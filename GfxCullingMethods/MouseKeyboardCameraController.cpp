#include <algorithm>
#include "Window.h"
#include "MouseKeyboardCameraController.h"
#include "MicrosecondsTimer.h"

MouseKeyboardCameraController::MouseKeyboardCameraController(Camera& camera) : m_Camera(camera), m_RotationAxisY(0.0f), m_Sensetive(DEFAULT_MOUSE_SENSETIVE) 
{
}

void MouseKeyboardCameraController::Update(Window& window)
{
    const float VELOCITY = 200.0f;//0.005f;

    static MicrosecondsTimer timer;
    __int64 mmsElapsed = timer.End();
    timer.Reset();
    timer.Begin();
    float delta = (double)mmsElapsed / 1000000.0;

    m_MouseCameraRotationActive = !!(GetAsyncKeyState(VK_RBUTTON));

    if (m_MouseCameraRotationActive)
    {
        unsigned long midCursorX = window.GetX() + window.GetWidth() / 2;
        unsigned long midCursorY = window.GetY() + window.GetHeight() / 2;

        long dX = window.GetCursorX() - midCursorX;
        long dY = window.GetCursorY() - midCursorY;

        window.AddCommand(SetCursorPosCommand(midCursorX, midCursorY));

        m_RotationAxisY += dX * m_Sensetive;
        m_RotationAxisX += dY * m_Sensetive;

        const float maxXAxisRotateAngle = glm::radians(70.0f);
        m_RotationAxisX = std::max(std::min(m_RotationAxisX, maxXAxisRotateAngle), -maxXAxisRotateAngle);

        m_Camera.SetRotation(glm::vec3(-m_RotationAxisX, m_RotationAxisY, 0.0f));
    }

    if (GetAsyncKeyState(VK_UP))
    {
        m_Camera.StepForward(VELOCITY * delta);
    }
    if (GetAsyncKeyState(VK_DOWN))
    {
        m_Camera.StepForward(-VELOCITY * delta);
    }
    if (GetAsyncKeyState(VK_LEFT))
    {
        m_Camera.StepLeft(VELOCITY * delta);
    }
    if (GetAsyncKeyState(VK_RIGHT))
    {
        m_Camera.StepLeft(-VELOCITY * delta);
    }

    m_Camera.UpdateProjection((float)window.GetWidth() / (float)window.GetHeight());
}
