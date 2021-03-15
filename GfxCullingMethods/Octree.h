#pragma once
#include "AABB.h"
#include "SuperMeshInstance.h"
#include "PagedObjPool.h"
#include "Camera.h"
#include "LightList.h"
#include <vector>
#include <functional>
#include <list>

//static octree
/*template<class payload_t>
class IndexBasedOctree
{
public:
    IndexBasedOctree() {}
    struct AABBwithPayload
    {
        AABBwithPayload() : lastGatherFrame(0), chIndexesSz(0) { memset(chIndexes, 0, sizeof(chIndexes)); }
        AABBwithPayload(AABB _aabb, payload_t pl) : lastGatherFrame(0), chIndexesSz(0), aabb(_aabb), payload(pl) { memset(chIndexes, 0, sizeof(chIndexes)); }



        AABB aabb;
        payload_t payload;

        //system vars
        unsigned long long lastGatherFrame;

        size_t chIndexesSz;
        size_t chIndexes[8];
        //
    };
    struct Node
    {
        AABB aabb;
        int aabbsBegin;
        int aabbsEnd;

        Node* childs;

        Node() : aabbsBegin(-1), aabbsEnd(-1)
        {
            FillChildsNull();
        }
        Node(size_t begin, size_t end) : aabbsBegin(begin), aabbsEnd(end)
        {
            FillChildsNull();
        }
        int InternalNodesEnd()
        {
            if (!childs)
                return aabbsEnd;
            else
                return childs[0].aabbsBegin;
        }
        void FillChildsNull()
        {
            childs = nullptr;
        }

        enum ChildTypeMask
        {
            Top_Child = 1 << 1,
            Right_Child = 1 << 2,
            Front_Child = 1
        };
        typedef PagedObjPool<Node[8], 312> NodesPool;

        void InitializeChilds(NodesPool& nodesPool)
        {
            const float halfSide = aabb.Side() * 0.5f;
            childs = new Node[8];//*nodesPool.Pop();
            
            childs[000].aabb = AABB(aabb.m_Min, aabb.m_Min + glm::vec3(halfSide, halfSide, halfSide));
            childs[0 | Top_Child | 0].aabb = AABB(childs[000].aabb.m_Min + glm::vec3(0.0f, halfSide, 0.0f), childs[000].aabb.m_Max + glm::vec3(0.0f, halfSide, 0.0f));
            childs[Right_Child | 0 | 0].aabb = AABB(childs[000].aabb.m_Min + glm::vec3(halfSide, 0.0f, 0.0f), childs[000].aabb.m_Max + glm::vec3(halfSide, 0.0f, 0.0f));
            childs[0 | 0| Front_Child].aabb = AABB(childs[000].aabb.m_Min + glm::vec3(0.0f, 0.0f, halfSide), childs[000].aabb.m_Max + glm::vec3(0.0f, 0.0f, halfSide));
            childs[Right_Child | 0 | Front_Child].aabb = AABB(childs[000].aabb.m_Min + glm::vec3(halfSide, 0.0f, halfSide), childs[000].aabb.m_Max + glm::vec3(halfSide, 0.0f, halfSide));
            childs[Right_Child | Front_Child | Top_Child].aabb = AABB(childs[000].aabb.m_Min + glm::vec3(halfSide, halfSide, halfSide), childs[000].aabb.m_Max + glm::vec3(halfSide, halfSide, halfSide));
            childs[0 | Top_Child | Front_Child].aabb = AABB(childs[000].aabb.m_Min + glm::vec3(0.0f, halfSide, halfSide), childs[000].aabb.m_Max + glm::vec3(0.0f, halfSide, halfSide));
            childs[Right_Child | Top_Child | 0].aabb = AABB(childs[000].aabb.m_Min + glm::vec3(halfSide, halfSide , 0.0f), childs[000].aabb.m_Max + glm::vec3(halfSide, halfSide, 0.0f));
        }

        bool CalcChildIndexes(const AABB& _aabb, size_t* childIndexes, size_t& indexesCount)
        {
            size_t prevVDbg = indexesCount;
            indexesCount = 0;
            for (size_t i = 0; i < 8; i++)
                if (childs[i].aabb.VolumeIntersect_(_aabb))
                {
                    *childIndexes = i;
                    indexesCount++;
                    childIndexes++;
                }
            return indexesCount != 0;
        }
        bool Leaf()
        {
            return childs == nullptr;
        }
    };
    struct TempChildIndexesCache
    {
        TempChildIndexesCache() : sz(0) {}
        size_t indexes[8];
        size_t sz;

    };

    void Partition(int begin, int end, int& pivot, const std::function<bool (int)>& cl) // pivot = end for the first half
    {
        int i = begin - 1;
        int j = end - 1;
        while (i < j)
        {
            i++;
            if (!cl(i))
            {
                while (i < j && !cl(j))
                    j--;
                std::swap(m_AABBIndexes[i], m_AABBIndexes[j]);
            }
        }
        if (!cl(i))
            i--;
        pivot = i + 1;
    }

#define MAX_DEPTH 6
#define LEAF_THRESHOLD 128
    void Build(Node* nd, int depth)
    {
        if (depth > MAX_DEPTH)
            return;
        if (nd->aabbsEnd - nd->aabbsBegin < LEAF_THRESHOLD)
            return;

        nd->InitializeChilds(m_NodesPool);

        AABB dbgbvs[8];
        for (size_t i = 0; i < 8; i++)
            dbgbvs[i] = nd->childs[i].aabb;

        size_t check = 0;
        for (size_t i = nd->aabbsBegin; i < nd->aabbsEnd; i++)
        {
            AABBwithPayload& aabbwp = AccessAABBByIndexIndex(i);

            if (!nd->CalcChildIndexes(aabbwp.aabb, aabbwp.chIndexes, aabbwp.chIndexesSz))
            {
                check = i;
                break;
            }
            
        }
        //AABBwithPayload& aabbwp = AccessAABBByIndexIndex(check);
        //bool dbgi = nd->aabb.VolumeIntersect(aabbwp.aabb);
        //nd->CalcChildIndexes(aabbwp.aabb, aabbwp.chIndexes, aabbwp.chIndexesSz);

        int pivot = 0;
        Partition(nd->aabbsBegin, nd->aabbsEnd, pivot, [&](int i) {return AccessAABBByIndexIndex(i).chIndexesSz > 1; });

        for (size_t j = 0; j < 8; j++)
        {
            int chBegin = pivot;
            Partition(pivot, nd->aabbsEnd, pivot, [&](int i) {return AccessAABBByIndexIndex(i).chIndexes[0] == j; });

            nd->childs[j].aabbsBegin = chBegin;
            nd->childs[j].aabbsEnd = pivot;
            Build(&nd->childs[j], depth + 1);
        }
    }
    void Build()
    {
        FillIndexes();
        m_AABB.ConvertToCube();

        m_Root = new Node(0, m_AABBs.size());
        m_Root->aabb = m_AABB;

        Build(m_Root, 1);
    }
    void Add(AABB aabb, payload_t pl)
    {
        AABBwithPayload aabbwp = { aabb, pl };
        m_AABBs.push_back(aabbwp);
        m_AABB.Extend(aabb);
    }
    AABBwithPayload& AccessAABBByIndexIndex(size_t i)
    {
        return m_AABBs[m_AABBIndexes[i]];
    }

    void FillIndexes()
    {
        m_AABBIndexes.clear();
        m_AABBIndexes.resize(m_AABBs.size());
        for (size_t i = 0; i < m_AABBIndexes.size(); i++)
            m_AABBIndexes[i] = i;
    }

    void GatherVisiblePayload(Node* node, Camera::Frustum& frustum, std::vector<payload_t>& payloads, int lvl)
    {
        if (frustum.Test(node->aabb))
        {
            for (size_t i = node->aabbsBegin; i < node->InternalNodesEnd(); i++) // check internal nodes end
                payloads.push_back(AccessAABBByIndexIndex(i).payload);

            if (node->childs)
            {
                for (size_t i = 0; i < 8; i++)
                {
                    GatherVisiblePayload(&node->childs[i], frustum, payloads, lvl + 1);
                }
            }
        }
    }
    void GatherVisiblePayload(Camera::Frustum& frustum, std::vector<payload_t>& payloads)
    {
        GatherVisiblePayload(m_Root, frustum, payloads, 0);
    }
    size_t GetManagedBVCount()
    {
        return m_AABBs.size();
    }

    void Clear() // fully destruct i.e. allocators along with all allocated data would be completely terminated
    {
        if (m_Root)
            delete m_Root;
        m_Root = nullptr;
        m_NodesPool.Clear();
    }
private:
    Node* m_Root;
    typedef PagedObjPool<Node[8], 312> NodesPool;
    NodesPool m_NodesPool;
    std::vector<AABBwithPayload> m_AABBs;
    std::vector<size_t> m_AABBIndexes;
    AABB m_AABB;
};*/


//memory tight octree
template<class payload_t>
class MemoryTightOctree
{
public:
    MemoryTightOctree() {}
    struct AABBwithPayload
    {
        AABBwithPayload() : lastGatherFrame(0), cullMask(0) { }
        AABBwithPayload(AABB _aabb, payload_t pl) : lastGatherFrame(0), aabb(_aabb), payload(pl), cullMask(0) {  }

        AABB aabb;
        payload_t payload;

        //system vars
        uint8_t cullMask;
        unsigned long long lastGatherFrame;
    };
    struct Node
    {
        AABB aabb;
        std::list<AABBwithPayload> objs;

        Node* childs;

        Node()
        {
            FillChildsNull();
        }
        Node(size_t begin, size_t end) 
        {
            FillChildsNull();
        }

        void FillChildsNull()
        {
            childs = nullptr;
        }

        enum ChildTypeMask
        {
            Top_Child = 1 << 1,
            Right_Child = 1 << 2,
            Front_Child = 1
        };
        typedef PagedObjPool<Node[8], 312> NodesPool;

        void InitializeChilds(NodesPool& nodesPool)
        {
            const float halfSide = aabb.Side() * 0.5f;
            childs = new Node[8];//*nodesPool.Pop();

            childs[000].aabb = AABB(aabb.m_Min, aabb.m_Min + glm::vec3(halfSide, halfSide, halfSide));
            childs[0 | Top_Child | 0].aabb = AABB(childs[000].aabb.m_Min + glm::vec3(0.0f, halfSide, 0.0f), childs[000].aabb.m_Max + glm::vec3(0.0f, halfSide, 0.0f));
            childs[Right_Child | 0 | 0].aabb = AABB(childs[000].aabb.m_Min + glm::vec3(halfSide, 0.0f, 0.0f), childs[000].aabb.m_Max + glm::vec3(halfSide, 0.0f, 0.0f));
            childs[0 | 0 | Front_Child].aabb = AABB(childs[000].aabb.m_Min + glm::vec3(0.0f, 0.0f, halfSide), childs[000].aabb.m_Max + glm::vec3(0.0f, 0.0f, halfSide));
            childs[Right_Child | 0 | Front_Child].aabb = AABB(childs[000].aabb.m_Min + glm::vec3(halfSide, 0.0f, halfSide), childs[000].aabb.m_Max + glm::vec3(halfSide, 0.0f, halfSide));
            childs[Right_Child | Front_Child | Top_Child].aabb = AABB(childs[000].aabb.m_Min + glm::vec3(halfSide, halfSide, halfSide), childs[000].aabb.m_Max + glm::vec3(halfSide, halfSide, halfSide));
            childs[0 | Top_Child | Front_Child].aabb = AABB(childs[000].aabb.m_Min + glm::vec3(0.0f, halfSide, halfSide), childs[000].aabb.m_Max + glm::vec3(0.0f, halfSide, halfSide));
            childs[Right_Child | Top_Child | 0].aabb = AABB(childs[000].aabb.m_Min + glm::vec3(halfSide, halfSide, 0.0f), childs[000].aabb.m_Max + glm::vec3(halfSide, halfSide, 0.0f));
        }

        bool CalcChildIndexes(const AABB& _aabb, size_t* childIndexes, size_t& indexesCount)
        {
            indexesCount = 0;
            for (size_t i = 0; i < 8; i++)
                if (childs[i].aabb.VolumeIntersect(_aabb))
                {
                    *childIndexes = i;
                    indexesCount++;
                    childIndexes++;
                }
            return indexesCount != 0;
        }
        bool Leaf()
        {
            return childs == nullptr;
        }
    };
    struct TempChildIndexesCache
    {
        TempChildIndexesCache() : sz(0) {}
        size_t indexes[8];
        size_t sz;

    };

#define MAX_DEPTH 10
#define LEAF_THRESHOLD 2
    void Build(Node* nd, int depth)
    {
       
    }
    void Build()
    {
    }
    void Propagate(Node* nd, AABBwithPayload aabbwp, int lvl)
    {
        if (lvl > MAX_DEPTH)
            return;
        
        if (nd->objs.size() + 1 <= LEAF_THRESHOLD)
        {
            nd->objs.push_back(aabbwp);
            return;
        }
        size_t indexes[8];
        size_t indexesSz = 0;
        if (!nd->childs)
        {
            nd->objs.push_back(aabbwp);

            nd->InitializeChilds(m_NodesPool);

            auto it = nd->objs.begin();
            while ( it != nd->objs.end())
            {
                AABBwithPayload& bv = *it;

                nd->CalcChildIndexes(bv.aabb, indexes, indexesSz);
 
                if (indexesSz == 1)
                {
                    auto curr = it;
                    it++;
 
                    Propagate(&nd->childs[indexes[0]], bv, lvl + 1);

                    nd->objs.erase(curr);
                    continue;
                }
                else
                {
                    uint8_t cullMask = 0;
                    for (size_t i = 0; i < indexesSz; i++)
                        cullMask = cullMask | (1 << indexes[i]);
                    (*it).cullMask = cullMask;
                }
                it++;
            }
        }
        else
        {
            nd->CalcChildIndexes(aabbwp.aabb, indexes, indexesSz);

            if (indexesSz == 1)
            {
                Propagate(&nd->childs[indexes[0]], aabbwp, lvl + 1);
            }
            else
            {
                uint8_t cullMask = 0;
                for (size_t i = 0; i < indexesSz; i++)
                    cullMask = cullMask | (1 << indexes[i]);
                aabbwp.cullMask = cullMask;

                nd->objs.push_back(aabbwp);
            }
        }
    }
    void Add(AABB sceneAABB, AABB aabb, payload_t pl)
    {
        AABBwithPayload aabbwp = { aabb, pl };
        if (!m_Root)
        {
            m_Root = new Node();
            m_Root->aabb = sceneAABB;
        }

        Propagate(m_Root, aabbwp, 1);
    }
    AABBwithPayload& AccessAABBByIndexIndex(size_t i)
    {
        return m_AABBs[m_AABBIndexes[i]];
    }

    void FillIndexes()
    {
        m_AABBIndexes.clear();
        m_AABBIndexes.resize(m_AABBs.size());
        for (size_t i = 0; i < m_AABBIndexes.size(); i++)
            m_AABBIndexes[i] = i;
    }

    void GatherVisiblePayload(Node* node, Camera::Frustum& frustum, std::vector<payload_t>& payloads, int reqLvl, int lvl)
    {
        {
            uint8_t cullMask = 0;

            if (node->childs)
            {
                for (size_t i = 0; i < 8; i++)
                {
                    if (frustum.Test(node->childs[i].aabb))
                    {
                        cullMask = cullMask | (1 << i);
                        GatherVisiblePayload(&node->childs[i], frustum, payloads, reqLvl, lvl + 1);
                    }
                }
            }
            //if (lvl == reqLvl)
            {
                for (AABBwithPayload& aabbwp : node->objs) // check internal nodes end
                    if ((aabbwp.cullMask & cullMask) || !node->childs)
                        payloads.push_back(aabbwp.payload);
                return;
            }
        }
    }
    void GatherVisiblePayload(Camera::Frustum& frustum, std::vector<payload_t>& payloads, int reqLvl)
    {
        GatherVisiblePayload(m_Root, frustum, payloads, 1, 0);
    }

    void DebugGatherAABBsLevel(Camera::Frustum& frustum, Node* node, std::vector<AABB>& aabbs, int& childsOcc, int currLevel, int reqLevel = 0) // remove in future/final version
    {
        if (node->childs)
        {
            for (size_t i = 0; i < 8; i++)
            {

                if (currLevel == reqLevel && frustum.Test(node->childs[i].aabb))
                {
                    aabbs.push_back(node->childs[i].aabb);
                    continue;
                }

                if (currLevel < reqLevel)
                {
                    DebugGatherAABBsLevel(frustum, &node->childs[i], aabbs, childsOcc, currLevel + 1, reqLevel);
                }
            }
        }
        else
            childsOcc++;
    }
    void DebugGatherAABBsLevel(Camera::Frustum& frust, std::vector<AABB>& aabbs, int& childsOcc, int reqLevel = 0) // remove in future/final version
    {
        childsOcc = 0;
        if (reqLevel == 0)
            aabbs.push_back(m_Root->aabb);
        DebugGatherAABBsLevel(frust, m_Root, aabbs, childsOcc, 1, reqLevel);
    }

    size_t GetManagedBVCount()
    {
        return m_AABBs.size();
    }

    void Clear() // fully destruct i.e. allocators along with all allocated data would be completely terminated
    {
        if (m_Root)
            delete m_Root;
        m_Root = nullptr;
        m_NodesPool.Clear();
    }
private:
    Node* m_Root;
    typedef PagedObjPool<Node[8], 312> NodesPool;
    NodesPool m_NodesPool;
    std::vector<AABBwithPayload> m_AABBs;
    std::vector<size_t> m_AABBIndexes;
    AABB m_AABB;
};

template<class payload_t>
class PerfWideOctree
{
public:
    PerfWideOctree() {}
    struct AABBwithPayload
    {
        AABBwithPayload() : lastGatherFrame(-1), cullMask(0) { }
        AABBwithPayload(AABB _aabb, payload_t pl) : lastGatherFrame(-1), aabb(_aabb), payload(pl), cullMask(0) {  }

        AABB aabb;
        payload_t payload;

        //system vars
        uint8_t cullMask;
        int64_t lastGatherFrame;
    };
    struct Node
    {
        AABB aabb;
        //std::list<size_t> objs;
        LightList<size_t> objs;

        Node* childs;

        Node()
        {
            FillChildsNull();
        }
        Node(size_t begin, size_t end)
        {
            FillChildsNull();
        }

        void FillChildsNull()
        {
            childs = nullptr;
        }

        enum ChildTypeMask
        {
            Top_Child = 1 << 1,
            Right_Child = 1 << 2,
            Front_Child = 1
        };
        typedef PagedObjPool<Node[8], 312> NodesPool;

        void InitializeChilds(NodesPool& nodesPool, LightList<size_t>::Allocator& listNodesPool)
        {
            const float halfSide = aabb.Side() * 0.5f;
            childs = *nodesPool.Pop();

            childs[000].aabb = AABB(aabb.m_Min, aabb.m_Min + glm::vec3(halfSide, halfSide, halfSide));
            childs[0 | Top_Child | 0].aabb = AABB(childs[000].aabb.m_Min + glm::vec3(0.0f, halfSide, 0.0f), childs[000].aabb.m_Max + glm::vec3(0.0f, halfSide, 0.0f));
            childs[Right_Child | 0 | 0].aabb = AABB(childs[000].aabb.m_Min + glm::vec3(halfSide, 0.0f, 0.0f), childs[000].aabb.m_Max + glm::vec3(halfSide, 0.0f, 0.0f));
            childs[0 | 0 | Front_Child].aabb = AABB(childs[000].aabb.m_Min + glm::vec3(0.0f, 0.0f, halfSide), childs[000].aabb.m_Max + glm::vec3(0.0f, 0.0f, halfSide));
            childs[Right_Child | 0 | Front_Child].aabb = AABB(childs[000].aabb.m_Min + glm::vec3(halfSide, 0.0f, halfSide), childs[000].aabb.m_Max + glm::vec3(halfSide, 0.0f, halfSide));
            childs[Right_Child | Front_Child | Top_Child].aabb = AABB(childs[000].aabb.m_Min + glm::vec3(halfSide, halfSide, halfSide), childs[000].aabb.m_Max + glm::vec3(halfSide, halfSide, halfSide));
            childs[0 | Top_Child | Front_Child].aabb = AABB(childs[000].aabb.m_Min + glm::vec3(0.0f, halfSide, halfSide), childs[000].aabb.m_Max + glm::vec3(0.0f, halfSide, halfSide));
            childs[Right_Child | Top_Child | 0].aabb = AABB(childs[000].aabb.m_Min + glm::vec3(halfSide, halfSide, 0.0f), childs[000].aabb.m_Max + glm::vec3(halfSide, halfSide, 0.0f));
            for (size_t i = 0; i < 8; i++)
                childs[i].objs = LightList<size_t>(&listNodesPool);
        }

        bool CalcChildIndexes(const AABB& _aabb, size_t* childIndexes, size_t& indexesCount)
        {
            indexesCount = 0;
            for (size_t i = 0; i < 8; i++)
                if (childs[i].aabb.VolumeIntersect_(_aabb))
                {
                    *childIndexes = i;
                    indexesCount++;
                    childIndexes++;
                }
            return indexesCount != 0;
        }
        bool Leaf()
        {
            return childs == nullptr;
        }
    };
    struct TempChildIndexesCache
    {
        TempChildIndexesCache() : sz(0) {}
        size_t indexes[8];
        size_t sz;

    };

#define MAX_DEPTH 7
#define LEAF_THRESHOLD 1
    void Build(Node* nd, int depth)
    {

    }
    void Build()
    {
    }
    void Propagate(Node* nd, size_t aabbwpIdx, int lvl)
    {
        if (!nd->childs && (nd->objs.size() + 1 <= LEAF_THRESHOLD || lvl == MAX_DEPTH))
        {
            nd->objs.push_back(aabbwpIdx);
            return;
        }
        size_t indexes[8];
        size_t indexesSz = 0;
        if (!nd->childs)
        {
            nd->objs.push_back(aabbwpIdx);

            nd->InitializeChilds(m_NodesPool, m_ListNodesPool);

            for (size_t idx : nd->objs)
            {
                AABBwithPayload& bv = m_AABBs[idx];

                nd->CalcChildIndexes(bv.aabb, indexes, indexesSz);

                for (size_t i = 0; i < indexesSz; i++)
                {
                    Propagate(&nd->childs[indexes[i]], idx, lvl + 1);
                }
            }
            nd->objs.clear();
        }
        else
        {
            nd->CalcChildIndexes(m_AABBs[aabbwpIdx].aabb, indexes, indexesSz);

            for (size_t i = 0; i < indexesSz; i++)
                Propagate(&nd->childs[indexes[i]], aabbwpIdx, lvl + 1);
        }
    }
    void Add(AABB sceneAABB, AABB aabb, payload_t pl)
    {
        AABBwithPayload aabbwp = { aabb, pl };
        if (!m_Root)
        {
            m_Root = new Node();
            m_Root->objs = LightList<size_t>(&m_ListNodesPool);
            m_Root->aabb = sceneAABB;
        }
        m_AABBs.push_back(aabbwp);

        Propagate(m_Root, m_AABBs.size() - 1, 1);
    }

    void GatherVisiblePayload(Node* node, Camera::Frustum& frustum, std::vector<payload_t>& payloads, int reqLvl, int lvl, int64_t frameIndex)
    {
        {
            if (node->childs)
            {
                for (size_t i = 0; i < 8; i++)
                {
                    if (frustum.Test(node->childs[i].aabb))
                    {
                        GatherVisiblePayload(&node->childs[i], frustum, payloads, reqLvl, lvl + 1, frameIndex);
                    }
                }
                return;
            }
            //if (lvl == reqLvl)
            {
                for (size_t aabbwpIdx : node->objs) // check internal nodes end
                {
                    AABBwithPayload& aabbwp = m_AABBs[aabbwpIdx];

                    if ((aabbwp.lastGatherFrame < frameIndex))
                    {
                        payloads.push_back(aabbwp.payload);

                        aabbwp.lastGatherFrame = frameIndex;
                    }
                }
                return;
            }
        }
    }
    void GatherVisiblePayload(Camera::Frustum& frustum, std::vector<payload_t>& payloads, int reqLvl, int64_t frameIndex)
    {
        GatherVisiblePayload(m_Root, frustum, payloads, 1, 0, frameIndex);
    }

    void DebugGatherAABBsLevel(Camera::Frustum& frustum, Node* node, std::vector<AABB>& aabbs, int& childsOcc, int currLevel, int reqLevel = 0) // remove in future/final version
    {
        if (node->childs)
        {
            for (size_t i = 0; i < 8; i++)
            {

                if (currLevel == reqLevel && frustum.Test(node->childs[i].aabb))
                {
                    aabbs.push_back(node->childs[i].aabb);
                    continue;
                }

                if (currLevel < reqLevel)
                {
                    DebugGatherAABBsLevel(frustum, &node->childs[i], aabbs, childsOcc, currLevel + 1, reqLevel);
                }
            }
        }
        else
            childsOcc++;
    }
    void DebugGatherAABBsLevel(Camera::Frustum& frust, std::vector<AABB>& aabbs, int& childsOcc, int reqLevel = 0) // remove in future/final version
    {
        childsOcc = 0;
        if (reqLevel == 0)
            aabbs.push_back(m_Root->aabb);
        DebugGatherAABBsLevel(frust, m_Root, aabbs, childsOcc, 1, reqLevel);
    }

    size_t GetManagedBVCount()
    {
        return m_AABBs.size();
    }

    void Clear() // fully destruct i.e. allocators along with all allocated data would be completely terminated
    {
        if (m_Root)
            delete m_Root;
        m_Root = nullptr;
        m_NodesPool.Clear();
        m_ListNodesPool.Clear();
    }
private:
    Node* m_Root;
    typedef PagedObjPool<Node[8], 312> NodesPool;
    NodesPool m_NodesPool;
    std::vector<AABBwithPayload> m_AABBs;
    LightList<size_t>::Allocator m_ListNodesPool;

    AABB m_AABB;
};