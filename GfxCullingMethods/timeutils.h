#pragma once
#include <string>

struct HighResolutionFormattedTime
{
    HighResolutionFormattedTime(long seconds, long miliseconds, long microseconds) : m_Seconds(seconds),
                                                                                     m_Miliseconds(miliseconds),
                                                                                     m_Microseconds(microseconds)
    {}

    long m_Seconds;
    long m_Miliseconds;
    long m_Microseconds;
};

unsigned int GetMilisecondsSinceProgramStart();
long long GetPerfomanceCounterMicroseconds();
HighResolutionFormattedTime GetFormattedTimeForPerfomanceCounterMicrosecons(long long microseconds);
std::string GetFormattedTimeString(const HighResolutionFormattedTime& formattedTimeStruct);