/* LICENSE BEGIN
* Copyright <2021> <Ahafontsev Semen aka risenow> *

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files 
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, 
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  LICENSE END
*/

#pragma once
#include "AABB.h"
#include "Camera.h"
#include "gfxutils.h"
#include "ObjPool.h"
#include <vector>

// implemented building technique is described in paper "On fast construction of SAH based BVHs" Ingo Wald 2007
template<class payload_t>
class BVH
{
public:
    struct AABBwithPayload
    {
        AABB aabb; 
        payload_t payload;
    };
    BVH() {}
    BVH(const std::vector<AABBwithPayload>& aabbs, AABB aabb, AABB caabb)
    {
        m_AABB = aabb;
        m_CAABB = caabb;
        m_AABBs = aabbs;
    }
    struct Node
    {
        Node() : aabbsBegin(-1), aabbsEnd(-1)
        {
            for (Node*& nd : childs)
                nd = nullptr;
        }
        Node(int begin, int end) : aabbsBegin(begin), aabbsEnd(end)
        {
            for (Node*& nd : childs)
                nd = nullptr;
        }
        bool IsLeaf()
        {
            return childs[0] == nullptr && childs[1] == nullptr;
        }

        AABB aabb;
        Node* childs[2];
        int aabbsBegin;
        int aabbsEnd;
    };

    struct Bin
    {
        Bin() : objNum(0), leftArea(0.0f), leftCount(0) {}

        int objNum;
        AABB realBV; 
        AABB cBV; //centroids BV
        double leftArea;
        int leftCount;
    };

    void FillIndexes()
    {
        m_AABBIndexes.clear();
        m_AABBIndexes.resize(m_AABBs.size());
        for (size_t i = 0; i < m_AABBIndexes.size(); i++)
            m_AABBIndexes[i] = i;
    }

    AABBwithPayload& AccessAABBByIndexIndex(size_t i)
    {
        return m_AABBs[m_AABBIndexes[i]];
    }
    void Add(AABB aabb, payload_t payload)
    {
        m_AABB.Extend(aabb);
        m_CAABB.Extend(aabb.m_Centroid);
        m_AABBs.push_back({ aabb, payload });
    }
    AABB StretchAABB(AABB aabb)
    {
        static const glm::vec3 epsOffset = glm::vec3(0.001, 0.001, 0.001);

        aabb.m_Max += epsOffset;
        aabb.m_Min -= epsOffset;

        return aabb;
    }
    //16
#define BIN_COUNT 8
    void Build(Node* node, AABB caabb) //caabb = centroids BV
    {
        assert(caabb.IsValid());

        int begin = node->aabbsBegin;
        int end = node->aabbsEnd;

        assert(end - begin >= 1);

        if (end - begin <= 1)
            return;

        Bin bins[BIN_COUNT];
        float caabbSizes[3] = { caabb.m_Max.x - caabb.m_Min.x, caabb.m_Max.y - caabb.m_Min.y, caabb.m_Max.z - caabb.m_Min.z };
        size_t maxSzIndex = 4;
        float maxSz = -FLT_MAX;
        for (size_t i = 0; i < 3; i++)
        {
            if (caabbSizes[i] > maxSz)
            {
                maxSz = caabbSizes[i];
                maxSzIndex = i;
            }
        }
        glm::vec3 binAxis = GetAxis(maxSzIndex);

        float caabbMaxP = glm::dot(binAxis, caabb.m_Max);
        float caabbMinP = glm::dot(binAxis, caabb.m_Min);
        const float k0 = BIN_COUNT / (caabbMaxP - caabbMinP);

        for (int i = begin; i < end; i++)
        {
            AABBwithPayload& aabb = AccessAABBByIndexIndex(i);
            float dbgIndex = k0 * (glm::dot(aabb.aabb.m_Centroid, binAxis) - caabbMinP);
            size_t binIndex = k0 * (glm::dot(aabb.aabb.m_Centroid, binAxis) - caabbMinP);
            assert(binIndex < BIN_COUNT);

            Bin& currBin = bins[binIndex];
            currBin.objNum++;
            currBin.cBV.Extend(aabb.aabb.m_Centroid);
            currBin.realBV.Extend(aabb.aabb);
        }
        static const glm::vec3 epsOffset = glm::vec3(0.001, 0.001, 0.001);
        for (Bin& bin : bins)
        {
            bin.cBV.m_Max += epsOffset;
            bin.cBV.m_Min -= epsOffset;
            bin.realBV.m_Max += epsOffset;
            bin.realBV.m_Min -= epsOffset;
        }

        AABB temp;
        int leftCount = 0;
        for (int i = 1; i < BIN_COUNT; i++)
        {
            temp.Extend(bins[i - 1].realBV);
            bins[i].leftArea = temp.Area();
            leftCount += bins[i - 1].objNum;
            bins[i].leftCount = leftCount;
        }
        int splitIndex = BIN_COUNT;
        double minCost = FLT_MAX;
        int rightCount = 0;
        temp = AABB();
        for (int i = BIN_COUNT - 1; i >= 0; i--) 
        {
            temp.Extend(bins[i].realBV);
            double rightArea = temp.Area();
            rightCount += bins[i].objNum;
            double cost = bins[i].leftArea * bins[i].leftCount + rightArea * rightCount;
            if (cost < minCost)
            {
                minCost = cost;
                splitIndex = i;
            }
        }

        int j = end - 1;
        int i = begin;
        
        while (j >= 0 && i < j)
        {
            size_t binIndex1 = k0 * (glm::dot(AccessAABBByIndexIndex(i).aabb.m_Centroid, binAxis) - caabbMinP);
            size_t binIndex2 = k0 * (glm::dot(AccessAABBByIndexIndex(j).aabb.m_Centroid, binAxis) - caabbMinP);

            if (binIndex1 >= splitIndex)
            {
                while (i < j && binIndex2 >= splitIndex)
                {
                    j--;
                    binIndex2 = k0 * (glm::dot(AccessAABBByIndexIndex(j).aabb.m_Centroid, binAxis) - caabbMinP);
                }
                std::swap(m_AABBIndexes[i], m_AABBIndexes[j]);
            }   
            i++;
        }
        if (i > j)
            i = j;

        size_t binIndex = k0 * (glm::dot(AccessAABBByIndexIndex(i).aabb.m_Centroid, binAxis) - caabbMinP);
        if (binIndex >= splitIndex)
            i--;
        else
+            j++;
        //i j < end > begin

        bool isLeaf = i < begin || j >= end;
        if (isLeaf)
            return;

        temp = AABB();
        AABB ctemp;
        if (i >= begin)
        {
            for (int k = 1; k <= splitIndex; k++)
            {
                temp.Extend(bins[k - 1].realBV);
                ctemp.Extend(bins[k - 1].cBV);
            }

            node->childs[0] = m_NodesPool.Pop();
            new (node->childs[0]) Node(begin, i + 1);
            node->childs[0]->aabb = temp;
            Build(node->childs[0], ctemp);
        }
        else
            node->childs[0] = nullptr;

        if (j < end)
        {
            temp = AABB();
            ctemp = AABB();
            for (int k = BIN_COUNT - 1; k >= splitIndex; k--)
            {
                temp.Extend(bins[k].realBV);
                ctemp.Extend(bins[k].cBV);
            }

            node->childs[1] = m_NodesPool.Pop();
            new (node->childs[1]) Node(j, end);
            node->childs[1]->aabb = temp;
            Build(node->childs[1], ctemp);
        }
        else
            node->childs[1] = nullptr;

    }
    void GatherAllPayload(std::vector<payload_t>& payloads)
    {
        for (AABBwithPayload& aabbwp : m_AABBs)
            payloads.push_back(aabbwp.payload);
    }
    void GatherVisiblePayload(Node* node, Camera::Frustum& frustum, std::vector<payload_t>& payloads, int lvl)
    {
        if (frustum.Test(node->aabb))
        {
            if (node->IsLeaf())
            {
                for (size_t i = node->aabbsBegin; i < node->aabbsEnd; i++)
                    payloads.push_back(AccessAABBByIndexIndex(i).payload);
                return;
            }
            if (node->childs[0])
                GatherVisiblePayload(node->childs[0], frustum, payloads, lvl + 1);
            if (node->childs[1])
                GatherVisiblePayload(node->childs[1], frustum, payloads, lvl + 1);
        }
    }
    void GatherVisiblePayload(Camera::Frustum& frustum, std::vector<payload_t>& payloads)
    {
        GatherVisiblePayload(m_Root, frustum, payloads, 0);
    }
    void DebugGatherAABBsLevel(Camera::Frustum& frustum, Node* node, std::vector<AABB>& aabbs, int& childsOcc, int currLevel, int reqLevel = 0) // remove in future/final version
    {
        if (node->IsLeaf())
        {
            childsOcc++;
            return;
        }

        for (size_t i = 0; i < 2; i++)
            if (node->childs[i])
            {
                
                if (currLevel == reqLevel && frustum.Test(node->childs[i]->aabb))
                {
                    aabbs.push_back(node->childs[i]->aabb);
                    continue;
                }

                if (currLevel < reqLevel)
                {
                    DebugGatherAABBsLevel(frustum, node->childs[i], aabbs, childsOcc, currLevel + 1, reqLevel);
                }
            }
    }
    void DebugGatherAABBsLevel(Camera::Frustum& frust, std::vector<AABB>& aabbs, int& childsOcc, int reqLevel = 0) // remove in future/final version
    {
        childsOcc = 0;
        if (reqLevel == 0)
            aabbs.push_back(m_Root->aabb);
        DebugGatherAABBsLevel(frust, m_Root, aabbs, childsOcc, 1, reqLevel);
    }
    void Build()
    {
        m_NodesPool.Reset(m_AABBs.size() * 2 + 1);
        m_Root = nullptr;

        FillIndexes();
        
        m_Root = m_NodesPool.Pop();
        new (m_Root) Node(0, m_AABBs.size());
        m_Root->aabb = m_AABB;

        Build(m_Root, StretchAABB(m_CAABB));
    }

    size_t GetBVCount()
    {
        return m_AABBs.size();
    }
    
    void Clear()
    {
        m_NodesPool.Clear();

        m_AABBs.clear();
        m_AABBIndexes.clear();
        m_CAABB = AABB();
        m_AABB = AABB();
        m_Root = nullptr;
    }

    void RecursiveNodesReturn(Node* nd)
    {
        m_NodesPool.Push(nd);
        for (Node* n : nd->childs)
            RecursiveNodesReturn(n);
    }
private:
    std::vector<AABBwithPayload> m_AABBs;
    std::vector<size_t> m_AABBIndexes;
    Node* m_Root;
    AABB m_CAABB;
    AABB m_AABB;
    Pool<Node> m_NodesPool;
};