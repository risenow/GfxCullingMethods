#pragma once

struct RenderStatistics
{
    RenderStatistics() : primCount(0), drawCallsCount(0) {}
    RenderStatistics(int _primCount, int _drawCalls) : primCount(_primCount), drawCallsCount(_drawCalls) {}

    int primCount;
    int drawCallsCount;

    RenderStatistics operator+(const RenderStatistics& stats)
    {
        return RenderStatistics(primCount + stats.primCount, drawCallsCount + stats.drawCallsCount);
    }
    void operator+=(const RenderStatistics& stats)
    {
        (*this) = (*this) + stats;
    }
};