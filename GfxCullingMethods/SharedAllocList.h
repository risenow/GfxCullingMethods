#pragma once
#include "PagedObjPool.h"

// dynamic pool allocated list that is not possible with std::list due to rebind reqierement
template<class T>
class SharedAllocList 
{
public:
    SharedAllocList() : m_Sentinel(nullptr), m_Size(0)
    {}
    
    struct Node
    {
        Node() : next(nullptr), prev(nullptr) {}
        Node* next;
        Node* prev;

        T payload;
    };

    typedef PagedObjPool<Node, 8192> Allocator;

    SharedAllocList(Allocator* pool) : m_NodesPool(pool), m_Size(0)
    {
        InitSentinel();
    }

    void InitSentinel()
    {
        m_Sentinel = m_NodesPool->Pop();
        m_Sentinel->next = m_Sentinel;
        m_Sentinel->prev = m_Sentinel;
    }
    Node* push_back(T pl)
    {
        if (!m_Sentinel)
            InitSentinel();

        Node* node = m_NodesPool->Pop();
        node->prev = m_Sentinel->prev;
        m_Sentinel->prev->next = node;
        node->next = m_Sentinel;
        m_Sentinel->prev = node;
        node->payload = pl;
        m_Size++;
        return node;
    }
    void clear()
    {
        m_Size = 0;
        if (m_Sentinel)
            m_NodesPool->Push(m_Sentinel);
        else
            return;
        Node* curr = m_Sentinel->next;
        while (curr != m_Sentinel)
        {
            if (curr)
                m_NodesPool->Push(curr);

            curr = curr->next;
        }
        m_Sentinel = nullptr;
    }
    bool empty()
    {
        return m_Sentinel->next == m_Sentinel;
    }
    class Iterator
    {
    public:
        Iterator(Node* c) : m_Curr(c) {}
        T operator *()
        {
            return m_Curr->payload;
        }
        bool operator != (const Iterator & other)
        {
            return m_Curr != other.m_Curr;
        }
        /*bool operator !=(const Iterator& other)
        {
            return m_Curr != other.m_Curr;
        }*/

        void operator++()
        {
            m_Curr = m_Curr->next;
        }
    private:
        Node* m_Curr;
    };

    Iterator begin()
    {
        if (!m_Sentinel)
            return Iterator(nullptr);
        return Iterator(m_Sentinel->next);
    }
    Iterator end()
    {
        return Iterator(m_Sentinel);
    }

    size_t size()
    {
        return m_Size;
    }
private:
    size_t m_Size;
    Node* m_Sentinel;
    Allocator* m_NodesPool;
};