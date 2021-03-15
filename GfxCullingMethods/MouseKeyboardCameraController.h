#pragma once
#include "Camera.h"
#include "Window.h"

static const float DEFAULT_MOUSE_SENSETIVE = 0.001f;

class MouseKeyboardCameraController
{
public:
	MouseKeyboardCameraController(Camera& camera);

	void Update(Window& window);
	Camera& GetCamera()
	{
		return m_Camera;
	}
	bool GetMouseCameraRotationActive() const
	{
		return m_MouseCameraRotationActive;
	}

private:
	float m_Sensetive;
	float m_RotationAxisY;
	float m_RotationAxisX;

    bool m_MouseCameraRotationActive;
	Camera& m_Camera;
};