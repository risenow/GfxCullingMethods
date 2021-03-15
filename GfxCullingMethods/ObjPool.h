#pragma once
#include "LinearStack.h"
#include <functional>

//it is not entirely correct since it relies on availability 
//of default constructor for given obj type
template<class T>
class Pool
{
public:
    Pool() : m_Objs(nullptr), m_NumObjs(0) {}
    Pool(size_t numObjs) : m_NumObjs(numObjs), m_ObjsStack(numObjs)
    {
        m_Objs = new T[numObjs];
        PopulateStack();
    }
    Pool(size_t numObjs, const std::function<void(T& obj)>& init) : m_NumObjs(numObjs), m_ObjsStack(numObjs)
    {
        m_Objs = new T[numObjs];
        for (size_t i = 0; i < m_NumObjs; i++)
            init(*(m_Objs + i));
        PopulateStack();
    }

    void Reset(size_t numObjs)
    {
        if (m_NumObjs == numObjs)
        {
            m_ObjsStack.ClearState();
            PopulateStack();
            return;
        }
        m_ObjsStack.Reset(numObjs);
        if (m_Objs)
            delete[] m_Objs;
        m_NumObjs = numObjs;
        m_Objs = numObjs ? new T[numObjs] : nullptr;
        PopulateStack();
    }

    void Clear()
    {
        m_ObjsStack.Clear();
        if (m_Objs)
            delete[] m_Objs;
        m_Objs = nullptr;

        m_NumObjs = 0;
    }

    T* Pop()
    {
        return m_ObjsStack.Pop();
    }
    void Push(T* obj)
    {
        m_ObjsStack.Push(obj);
    }
    size_t GetMaxObjectsNum()
    {
        return m_NumObjs;
    }
    inline bool Empty()
    {
        return m_ObjsStack.Empty();
    }
private:
    void PopulateStack()
    {
        T* currPointer = m_Objs;
        for (size_t i = 0; i < m_NumObjs; i++)
        {
            m_ObjsStack.Push(currPointer);
            currPointer++;
        }
    }

    size_t m_NumObjs;
    T* m_Objs;
    LinearStack<T*> m_ObjsStack;
};