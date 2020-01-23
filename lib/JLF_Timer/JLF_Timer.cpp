#include "JLF_Timer.h"

JLF_Timer::JLF_Timer(unsigned long delay)
{
    m_delay = delay;
    m_previousTime = millis();
    //    m_timeElapsed=false;
}
JLF_Timer::~JLF_Timer()
{
}

bool JLF_Timer::isTimeElapsed()
{
    bool timeElapsed = false;
    if (millis() - m_previousTime >= m_delay)
    {
        timeElapsed = true;
        m_previousTime = millis();
    }
    return timeElapsed;
}
void JLF_Timer::restart()
{
    m_previousTime = millis();
}
void JLF_Timer::restart(unsigned long delay)
{
    m_delay = delay;
    restart();
}