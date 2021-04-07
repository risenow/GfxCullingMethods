#include "timeutils.h"
#include <time.h>
#include <Windows.h>

unsigned int GetMilisecondsSinceProgramStart()
{
    static const float CLOCKS_PER_MILISEC = (float)CLOCKS_PER_SEC / 1000.0f;
    return (unsigned int)((float)clock() / CLOCKS_PER_MILISEC);
}

long long GetPerfomanceCounterFreq()
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    return freq.QuadPart;
}

double GetPerfCounterToMicrosecondsMultiplyCoef()
{
    static const double perfCounterToMicrosecondsMultiplyCoef = (double)1000000 / GetPerfomanceCounterFreq();
    return perfCounterToMicrosecondsMultiplyCoef;
}

long long GetPerfomanceCounterMicroseconds()
{
    LARGE_INTEGER perf;
    QueryPerformanceCounter(&perf);
    return (long long)(perf.QuadPart * GetPerfCounterToMicrosecondsMultiplyCoef());
}

HighResolutionFormattedTime GetFormattedTimeForPerfomanceCounterMicrosecons(long long microseconds)
{
    long microSecsLeft = 0;
    long secs = long(double(microseconds) / 1000000.0);
    microSecsLeft = microseconds - secs * 1000000;
    long milisecs = long(double(microSecsLeft) / 1000.0);
    microSecsLeft = microSecsLeft - milisecs * 1000;
    long microsecs = long(double(microSecsLeft));

    return HighResolutionFormattedTime(secs, milisecs, microseconds);
}

std::string GetFormattedTimeString(const HighResolutionFormattedTime& formattedTimeStruct)
{
    std::string result;
    result = std::to_string(formattedTimeStruct.m_Seconds) + "s " + 
             std::to_string(formattedTimeStruct.m_Miliseconds) + "ms " + 
             std::to_string(formattedTimeStruct.m_Microseconds) + "?s";
    return result;
}