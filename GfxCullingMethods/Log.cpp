#include "Log.h"
#include "timeutils.h"
#include <iostream>

RuntimeLog& RuntimeLog::GetInstance()
{
    static RuntimeLog s_RuntimeLog;
    return s_RuntimeLog;
}

RuntimeLog::RuntimeLog() : m_Enabled(true)
{
	m_DefaultStreamsSet = { &GetDefaultConsoleStream(), &GetDefaultFileStream() };
}
RuntimeLog::~RuntimeLog()
{
}
void RuntimeLog::SetMode(RuntimeLogMode mode)
{
    m_Mode = mode;
}
void RuntimeLog::SetEnabled(bool enabled)
{
    m_Enabled = enabled;
}
bool RuntimeLog::GetEnabled()
{
    return m_Enabled;
}
