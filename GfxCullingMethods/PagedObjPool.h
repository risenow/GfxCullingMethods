#pragma once
#include "ObjPool.h"
#include <vector>

template<class T, int pageSz>
class PagedObjPool
{
public:
    
    PagedObjPool() 
    {
        m_Pages.push_back(Pool<AllocData>(pageSz, [&](AllocData& obj) {obj.m_PageIndex = 0; }));
    }
    T* Pop()
    {
        for (size_t i = 0; i < m_Pages.size(); i++)
        {
            if (!m_Pages[i].Empty())
                return &m_Pages[i].Pop()->m_Data;
        }
        m_Pages.push_back(Pool<AllocData>(pageSz, [&](AllocData& obj) {obj.m_PageIndex = m_Pages.size() - 1; }));
        return &m_Pages.back().Pop()->m_Data;
        /*if (m_Pages[m_CurrFreePage].Empty())
        {
            if (m_CurrFreePage == m_Pages.size() - 1)
                m_Pages.push_back(Pool<AllocData>(pageSz, [&](AllocData& obj) {obj.m_PageIndex = m_CurrFreePage + 1; }));
            m_CurrFreePage++;
        }
        return &m_Pages[m_CurrFreePage].Pop()->m_Data;*/
    }
    void Push(T* obj)
    {
        AllocData* alloc = (AllocData*)((char*)obj - offsetof(struct AllocData, m_Data));
        m_Pages[alloc->m_PageIndex].Push(alloc);
    }

    void Clear()
    {
        for (Pool<AllocData>& pg : m_Pages)
            pg.Clear();
        m_Pages.resize(0);
    }
private:
    struct AllocData
    {
        size_t m_PageIndex;
        T m_Data;
    };

    std::vector<Pool<AllocData>> m_Pages;
};