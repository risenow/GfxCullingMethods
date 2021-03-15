#pragma once

struct Point2D
{
    Point2D() : m_X(0), m_Y(0) {}
    Point2D(int x, int y) : m_X(x), m_Y(y) {}

    Point2D operator + (const Point2D& p) { return Point2D(m_X + p.m_X, m_Y + p.m_Y); }
    Point2D operator - (const Point2D& p) { return Point2D(m_X - p.m_X, m_Y - p.m_Y); }

    int m_X;
    int m_Y;
};

struct BoundRect
{
    BoundRect() : m_Width(0), m_Height(0) {}
    BoundRect(Point2D topLeft, size_t width, size_t height) : m_TopLeft(topLeft),
                                                              m_Width(width), 
                                                              m_Height(height) {}

    Point2D GetTopLeft() { return m_TopLeft; }
    Point2D GetBotLeft() { return m_TopLeft + Point2D(0, -(int)m_Height); }
    Point2D GetTopRight() { return m_TopLeft + Point2D(m_Width, 0); }
    Point2D GetBotRight() { return m_TopLeft + Point2D(m_Width, -(int)m_Height); }

    Point2D m_TopLeft;
    size_t m_Width;
    size_t m_Height;
};