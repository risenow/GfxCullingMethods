#include "Window.h"
#include <cstring>
#include <thread>
#include "strutils.h"
#include "Log.h"
#include <limits>
#include <iostream>

Window::Window() : m_Width(DEFAULT_WINDOW_WIDTH), m_Height(DEFAULT_WINDOW_HEIGHT), 
                   m_X(DEFAULT_WINDOW_POS_X), m_Y(DEFAULT_WINDOW_POS_Y), 
                   m_Title(DEFAULT_WINDOW_TITLE), m_Closed(false), 
                   m_WindowMode(WindowMode_WINDOWED)
{
#ifdef MT_WINDOW
    std::thread windowThreadWorker(&Window::WindowThreadWorker, this);
    m_WindowThreadID = windowThreadWorker.get_id();
    windowThreadWorker.detach();
#else
    Window::WindowThreadWorker(this);
#endif
}

Window::Window(const std::string& title, unsigned int posX, unsigned int posY, 
               size_t width, size_t height) 
             : m_Width(width), m_Height(height), 
               m_X(posX), m_Y(posY), 
               m_Title(title), m_Closed(false), 
               m_WindowMode(WindowMode_WINDOWED)
{
#ifdef MT_WINDOW
    std::thread windowThreadWorker(&Window::WindowThreadWorker, this);
    m_WindowThreadID = windowThreadWorker.get_id();
    windowThreadWorker.detach();
#else
    Window::WindowThreadWorker(this);
#endif
}


/*Window::Window(const WindowAttributes& attributes) : m_Width(attributes.GetWidth()), m_Height(attributes.GetHeight()), 
                                                     m_X(attributes.GetPosX()), m_Y(attributes.GetPosY()), 
                                                     m_Title(attributes.GetTitle()), m_Closed(false), 
                                                     m_WindowMode(WINDOW_MODE_WINDOWED)
{
    std::thread windowThreadWorker(&Window::WindowThreadWorker, this);
    windowThreadWorker.detach();
}*/

void Window::InternalCreateWindow(Window* target)
{
	HINSTANCE hInstance = NULL;//GetConsoleHInstance(); //idk why i set console instance earlier mb there was a reason so this comment may stand here for a while

    const static std::wstring windowClassName = L"_D3DAPP_MAIN_WINDOW";

    WNDCLASS wc = { 0 };
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.hCursor = LoadCursor(hInstance, IDC_ARROW);
    wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wc.hInstance = hInstance;
    wc.lpfnWndProc = Window::WndProc;
    wc.lpszClassName = windowClassName.c_str();
    wc.style = CS_HREDRAW | CS_VREDRAW;

    if (!RegisterClass(&wc))
    {
        LOG(std::string("Failed to register window class"));
        exit(-1);
    }

    target->m_WindowHandle = CreateWindow(windowClassName.c_str(),
        strtowstr_fast(target->m_Title).c_str(),
        WS_OVERLAPPEDWINDOW,
        target->m_X, target->m_Y, target->m_Width, target->m_Height,
        NULL,
        NULL,
        hInstance,
        NULL);
    if (!target->m_WindowHandle)
    {
        LOG(std::string("Failed to create window"));
        exit(-1);
    }

#ifdef _DEBUG
    target->WindowCreationDebugOutput();
#endif

    SetWindowLongPtr(target->m_WindowHandle, GWLP_USERDATA, (LONG_PTR)target);

    ShowWindow(target->m_WindowHandle, SW_SHOW);
    UpdateWindow(target->m_WindowHandle);
}
void Window::WindowThreadWorker(Window* owner)
{
    InternalCreateWindow(owner);
#ifdef MT_WINDOW
    UpdateLoop(owner);
#endif
}
void Window::SetTitle(const std::string& title)
{
    m_Title = title;
    SetWindowText(m_WindowHandle, strtowstr_fast(m_Title).c_str());
}
void Window::Close()
{
    m_Closed = true;
}
bool Window::IsClosed() const
{
    return m_Closed;
}
bool Window::IsFocused() const
{
    return (GetForegroundWindow() == m_WindowHandle);
}
HWND Window::GetWindowHandle() const
{
    return m_WindowHandle;
}
WindowMode Window::GetWindowMode() const
{
    return m_WindowMode;
}

size_t Window::GetWidth() const
{
    return m_Width;
}
size_t Window::GetHeight() const
{
    return m_Height;
}
unsigned int Window::GetX() const
{
    return m_X;
}
unsigned int Window::GetY() const
{
    return m_Y;
}

BoundRect Window::GetBoundRect() const
{
    return BoundRect(GetTopLeft(), m_Width, m_Height);
}
Point2D Window::GetTopLeft() const
{
    return Point2D(m_X, m_Y);
}

std::string Window::GetTitle() const
{
    return m_Title;
}

unsigned long Window::GetCursorX() const
{
	return m_CursorX;
}
unsigned long Window::GetCursorY() const
{
	return m_CursorY;
}

void Window::SwitchMode()
{
    m_WindowMode = (m_WindowMode == WindowMode_WINDOWED) ? WindowMode_FULLSCREEN : WindowMode_WINDOWED;
}

void Window::WindowCreationDebugOutput()
{
    std::wstring logBuffer;
    LOG_BUFFER(std::string("Window created with next params: "),               logBuffer);
    LOG_BUFFER(std::string("Window Title: ") + m_Title,                        logBuffer);
    LOG_BUFFER(std::string("Window Pos X: ") + std::to_string(m_X),            logBuffer);
    LOG_BUFFER(std::string("Window Pos Y: ") + std::to_string(m_Y),            logBuffer);
    LOG_BUFFER(std::string("Window Width: ") + std::to_string(m_Width),        logBuffer);
    LOG_BUFFER(std::string("Window Height: ") + std::to_string(m_Height),      logBuffer);
	
	StringRuntimeLogStream logBufferStream(&logBuffer);
	popGetLogger().WriteUsingMode({ &logBufferStream }, logBuffer, RuntimeLogMode_None);
}
void Window::UpdateCursorPos()
{
	//popAssert(std::this_thread::get_id() == m_WindowThreadID);

	POINT p;
	GetCursorPos(&p);
	m_CursorX = p.x;
	m_CursorY = p.y;
}
void Window::UpdateLoop(Window* target)
{
	HWND windowHandle = target->GetWindowHandle();
    MSG msg;
	BOOL r;
	while (!target->IsClosed())
	{
		target->UpdateCursorPos();
		target->ExecuteWindowThreadCommands();
		while (r = PeekMessage(&msg, windowHandle, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

HINSTANCE Window::GetConsoleHInstance()
{
    const int titleCharBufferSize = 512;
    char consoleTitleTempCharBuffer[titleCharBufferSize];
    GetConsoleTitleA(consoleTitleTempCharBuffer, titleCharBufferSize);
    HWND hwndConsole = FindWindowA(NULL, consoleTitleTempCharBuffer);
#ifdef _WIN64
    HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLong(hwndConsole, GWLP_HINSTANCE));
#else
    HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLong(hwndConsole, GWL_HINSTANCE));
#endif
    return hInstance;
}

//extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    //if (ImGui_ImplWin32_WndProcHandler(hwnd, message, wparam, lparam))
    //    return true;

    switch (message)
    {
    case WM_SYSKEYDOWN:
        if (wparam == VK_RETURN)
        {
            if (HIWORD(lparam) & KF_ALTDOWN)
            {
                ((Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA))->SwitchMode();
            }
        }
        break;
	case WM_KEYDOWN:
		if (wparam == VK_ESCAPE)
		{
			((Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA))->Close();
		}

		break;
    case WM_SIZE:
    {
        switch (wparam)
        {
        case SIZE_RESTORED:
        case SIZE_MAXIMIZED:
        {
            Window* window = ((Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA));
            window->m_Width = LOWORD(lparam);
            window->m_Height = HIWORD(lparam);
            break;
        }
        }

        break;
    }
	case WM_MOVE:
	{
        Window* window = ((Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA));
        window->m_X = LOWORD(lparam);
        window->m_Y = HIWORD(lparam);

		switch (wparam)
		{
		case SIZE_RESTORED:
		{
			Window* window = ((Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA));
			window->m_X = LOWORD(lparam);
			window->m_Y = HIWORD(lparam);
			break;
		}
		}

		break;
	}
    case WM_CLOSE:
    {
        ((Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA))->Close();
        break;
    }

    case WM_PAINT:
    {
        break;
    }
    }

    return DefWindowProc(hwnd, message, wparam, lparam);
}

void Window::ExecuteWindowThreadCommands()
{
	m_WindowThreadCommandsMutex.lock();
	while (m_WindowThreadCommands.size())
	{
		m_WindowThreadCommands.top()->Execute();
		delete (m_WindowThreadCommands.top());
		m_WindowThreadCommands.pop();
	}
	m_WindowThreadCommandsMutex.unlock();
}