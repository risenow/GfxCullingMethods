#pragma once
#include <Windows.h>

class MicrosecondsTimer
{
public:
    MicrosecondsTimer()
    {
        QueryPerformanceFrequency(&m_Freq);
        Reset();
    }

    void Begin()
    {
        QueryPerformanceCounter(&m_Counter);
    }
    __int64 End()
    {
        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);

        LARGE_INTEGER microsecs;
        ZeroMemory(&microsecs, sizeof(LARGE_INTEGER));
        microsecs.QuadPart = counter.QuadPart - m_Counter.QuadPart;
        microsecs.QuadPart *= 1000000;
        microsecs.QuadPart /= m_Freq.QuadPart;
        Reset();

        return microsecs.QuadPart;
    }
    void Reset()
    {
        ZeroMemory(&m_Counter, sizeof(LARGE_INTEGER));
    }

private:
    LARGE_INTEGER m_Counter;
    LARGE_INTEGER m_Freq;
};