#pragma once
#include <Windows.h>
#include <string>
#include <stack>
#include <mutex>
#include "areahelper.h"

//#define MT_WINDOW

constexpr size_t DEFAULT_WINDOW_WIDTH = 512;
constexpr size_t DEFAULT_WINDOW_HEIGHT = 512;

constexpr unsigned int DEFAULT_WINDOW_POS_X = 0;
constexpr unsigned int DEFAULT_WINDOW_POS_Y = 0;

const std::string DEFAULT_WINDOW_TITLE = std::string("D3D11 Window");

enum WindowMode
{
    WindowMode_FULLSCREEN,
    WindowMode_WINDOWED,
    WindowMode_BORDERLESS,
    WINDOW_MODES_COUNT
};

class WindowThreadCommand
{
public:
	virtual void Execute() = 0;
};

class SetCursorPosCommand : public WindowThreadCommand
{
public:
	SetCursorPosCommand(unsigned long x, unsigned long y) : m_X(x), m_Y(y)
	{
	}

	virtual void Execute() override
	{
		SetCursorPos(m_X, m_Y);
	}
private:
	unsigned long m_X;
	unsigned long m_Y;
};

class SetCursorVisibilityCommand : public WindowThreadCommand
{
public:
	SetCursorVisibilityCommand(bool visibility) : m_Visible(visibility)
	{
	}

	virtual void Execute() override
	{
        CURSORINFO ci = { sizeof(CURSORINFO) };

        if (GetCursorInfo(&ci))
        {
            if (!m_Visible && ci.flags)
            {
                ShowCursor(false);
                ShowCursor(false);
            }
            if (m_Visible && !ci.flags)
            {
                ShowCursor(true);
                ShowCursor(true);
            }
        }
	}
private:
    static bool m_CurrentState;
	bool m_Visible;
};

//class for creation window in console app
class Window
{
public:
    Window();
    Window(const std::string& title, unsigned int posX, unsigned int posY, size_t width, size_t height);
    //Window(const WindowAttributes& attributes);

    void SetTitle(const std::string& title);
    void Close();

    bool IsClosed() const;
    bool IsFocused() const;
    HWND GetWindowHandle() const;
    WindowMode GetWindowMode() const;

    size_t GetWidth() const;
    size_t GetHeight() const;
    unsigned int GetX() const;
    unsigned int GetY() const;

    BoundRect GetBoundRect() const;
    Point2D GetTopLeft() const;

    std::string GetTitle() const;

	unsigned long GetCursorX() const;
	unsigned long GetCursorY() const;

    void SwitchMode();

    bool ExplicitUpdate() 
    {
        UpdateCursorPos();
        ExecuteWindowThreadCommands();

        MSG msg;
        if (::PeekMessage(&msg, m_WindowHandle, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            return true;
        }

        return true;
    }

	template<class T>
	void AddCommand(const T& command)
	{
		m_WindowThreadCommandsMutex.lock();
		m_WindowThreadCommands.push(new (T)(command));
		m_WindowThreadCommandsMutex.unlock();
	}
private:
	void UpdateCursorPos();
    void WindowCreationDebugOutput();

    static void WindowThreadWorker(Window* owner);
    static void InternalCreateWindow(Window* target);
    static HINSTANCE GetConsoleHInstance();

    static void UpdateLoop(Window* target);

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

	void ExecuteWindowThreadCommands();

	std::mutex m_WindowThreadCommandsMutex;
	std::stack<WindowThreadCommand*> m_WindowThreadCommands;

    unsigned int m_Width;
    unsigned int m_Height;

    unsigned int m_X;
    unsigned int m_Y;

	unsigned long m_CursorX;
	unsigned long m_CursorY;

	std::thread::id m_WindowThreadID;

    std::string m_Title;

    HWND m_WindowHandle;

    WindowMode m_WindowMode;

    bool m_Closed;
};