#pragma once

template<class T>
class LinearStack
{
public:
    LinearStack() : m_Stack(nullptr), m_LastIndex(-1), m_MaxElements(0), m_Size(0) {}
    LinearStack(size_t maxElements) : m_LastIndex(-1), m_MaxElements(maxElements), m_Size(0)
    {
        m_Stack = new T[maxElements];
    }

    void Push(T el)
    {
        assert((m_LastIndex + 1) < m_MaxElements);
        m_Stack[++m_LastIndex] = el;
        m_Size++;
    }
    T Pop()
    {
        assert(m_LastIndex >= 0);
        m_Size--;
        return m_Stack[m_LastIndex--];
    }
    void ClearState()
    {
        m_Size = 0;
        m_LastIndex = -1;
    }
    void Reset(size_t numEls)
    {
        assert(numEls >= 0);
        if (numEls == 0)
        {
            Clear();
            return;
        }
        if (numEls != m_MaxElements)
        {
            Clear();
            (*this) = LinearStack<T>(numEls);
            return;
        }
        ClearState();
    }
    inline bool Empty()
    {
        return m_Size <= 0;
    }
    void Clear()
    {
        if (m_Stack)
            delete[] m_Stack;
        m_Stack = nullptr;
        m_MaxElements = 0;
        ClearState();
    }
private:
    int m_Size;
    int m_MaxElements;
    int m_LastIndex;
    T* m_Stack;
};