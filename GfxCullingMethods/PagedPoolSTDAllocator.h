#pragma once
#include "PagedObjPool.h"
#include <vector>
#include <malloc.h>
#include <assert.h>

//it does not work, but would be here for next allocator development reference
template<class T, size_t pageSz>
class PagedPoolSTDAllocator
{
public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef PagedObjPool<T, pageSz> PagedPool;

    template<typename U>
    struct rebind {
        typedef PoolStackAllocator<U, pageSz> other;
    };

    PagedPoolSTDAllocator(PagedPool* pagedPl) : m_PagedPool(pagedPl) {}

    //not sure how to implement this in that situation, 
    template <class U, std::size_t M>
    inline explicit PagedPoolSTDAllocator(const PagedPoolSTDAllocator<U, M>& other)
    {}

    inline pointer address(reference r) { return &r; }
    inline const_pointer address(const_reference r) { return &r; }

    inline pointer allocate(size_type n)
    {
        assert(n == 1);

        return m_PagedPool->Pop();
    }

    inline void deallocate(T* p, size_type n) 
    {
        assert(n == 1);
        m_PagedPool->Push(p);
    }

    inline size_type max_size() 
    {
        return 1;
    }
private:
    PagedPool* m_PagedPool;
};