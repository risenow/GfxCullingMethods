#pragma once
#include "glm/glm.hpp"
#include "GraphicsDevice.h"
#include "SuperMeshInstance.h"
#include "DrawInstancedIndirectArgs.h"
#include "GraphicsBuffer.h"
#include "GraphicsConstantsBuffer.h"
#include "CullConsts.h"
#include <unordered_set>
#include <unordered_map>

struct Instance
{
    UINT batchIndex;
    glm::mat4x4 transform;
    glm::vec4 positionRadius;
};

struct BaseMesh
{
    Mesh* mesh;
    size_t instsCount;
};

class GPUDrivenRenderer
{
public:
    GPUDrivenRenderer(GraphicsDevice& device) 
    {
        m_CullShader = GraphicsShader::FromFile(device, GraphicsShaderType_Compute, L"cullcs.hlsl");
        CullConsts consts;
        ZeroMemory(&consts, sizeof(consts));
        m_CullConstantBuffer = GraphicsConstantsBuffer<CullConsts>(device, consts);
    }

    void Consume(GraphicsDevice& device, const std::vector<SuperMeshInstance>& insts)
    {
        m_DrawCount = 0;

        std::unordered_map<Mesh*, std::vector<Instance>> instancesPerMesh;
        for (const SuperMeshInstance& superInst : insts)
        {
            SuperMesh* superMesh = superInst.GetSuperMesh();
            
            for (size_t i = 0; i < superMesh->GetSubMeshesCount(); i++)
            {
                Mesh* mesh = superMesh->GetSubMesh(i);

                if (instancesPerMesh.find(mesh) == instancesPerMesh.end())
                    m_BaseMeshes.push_back({ mesh, 0 });
                instancesPerMesh[mesh].push_back({ 0, superInst.GetTransform(), mesh->GetAABB().GetSBB() });
            }
        }

        for (BaseMesh& baseMesh : m_BaseMeshes)
        {
            baseMesh.instsCount = instancesPerMesh[baseMesh.mesh].size();
        }

        size_t currentInstanceStart = 0;
        size_t batchIndex = 0;
        for (BaseMesh& baseMesh : m_BaseMeshes)
        {
            Mesh* mesh = baseMesh.mesh;
            std::vector<Instance>& instances = instancesPerMesh[mesh];

            m_DrawCount += instances.size();

            D3D11_DRAW_INSTANCED_INDIRECT_ARGS args;
            ZeroMemory(&args, sizeof(args));
            args.StartInstanceLocation = currentInstanceStart;
            args.StartVertexLocation = 0;
            args.VertexCountPerInstance = mesh->GetVertexCount();
            args.InstanceCount = 0;//instances.size(); to be filled in compute

            for (Instance& inst : instances)
            {
                inst.batchIndex = batchIndex;
                m_InstanceData.push_back(inst);
            }
            batchIndex++;
            currentInstanceStart += instances.size();

            m_IndirectArgs.push_back(args);
        }

        m_IndirectBuffer2 = RWByteAddressBuffer<D3D11_DRAW_INSTANCED_INDIRECT_ARGS>(device, m_IndirectArgs, GraphicsBuffer::MiscFlag_DrawIndirectArgs);
        m_InstancesBuffer = StructuredBuffer<Instance>(device, m_InstanceData);
        m_RefInstancesBuffer = StructuredBuffer<Instance>(device, m_InstanceData);
        m_RenderInstancesBuffer = RWStructuredBuffer<Instance>(device, m_InstanceData);
        m_IndirectBuffer = RWByteAddressBuffer<D3D11_DRAW_INSTANCED_INDIRECT_ARGS>(device, m_IndirectArgs, GraphicsBuffer::MiscFlag_DrawIndirectArgs);

    }
    void OnFrameBegin(GraphicsDevice& device)
    {
        device.GetD3D11DeviceContext()->CopyResource(m_IndirectBuffer.GetBuffer(), m_IndirectBuffer2.GetBuffer());
        //device.GetD3D11DeviceContext()->UpdateSubresource((ID3D11Resource*)m_IndirectBuffer2.GetBuffer(), 0, nullptr, m_IndirectArgs.data(), sizeof(D3D11_DRAW_INSTANCED_INDIRECT_ARGS) * m_IndirectArgs.size(), 0);
    }

    void Cull(GraphicsDevice& device, Camera& camera)
    {
        Camera::Frustum fr = Camera::Frustum(camera);

        CullConsts consts;
        consts.drawCount = m_DrawCount;
        consts.frLeft = fr.planes[0].AsVec();
        consts.frRight = fr.planes[1].AsVec();
        consts.frTop = fr.planes[2].AsVec();
        consts.frBottom = fr.planes[3].AsVec();

        m_CullConstantBuffer.Update(device, consts);
        m_CullConstantBuffer.Bind(device, GraphicsShaderMask_Compute);

        ID3D11ShaderResourceView* refInstancesSRV = m_RefInstancesBuffer.GetSRV();
        ID3D11UnorderedAccessView* uavs[] = { m_RenderInstancesBuffer.GetUAV(), m_IndirectBuffer.GetUAV() };

        unsigned int counts[2] = {0, 0};
        device.GetD3D11DeviceContext()->CSSetShaderResources(0, 1, &refInstancesSRV);
        device.GetD3D11DeviceContext()->CSSetUnorderedAccessViews(0, 2, uavs, nullptr);

        m_CullShader.Bind(device);
        
        device.GetD3D11DeviceContext()->Dispatch(ceil((float)m_InstanceData.size() / 256.0f), 1, 1);

        refInstancesSRV = nullptr;
        uavs[0] = nullptr;
        uavs[1] = nullptr;
        device.GetD3D11DeviceContext()->CSSetShaderResources(0, 1, &refInstancesSRV);
        device.GetD3D11DeviceContext()->CSSetUnorderedAccessViews(0, 2, uavs, nullptr);

        device.GetD3D11DeviceContext()->CopyResource(m_InstancesBuffer.GetBuffer(), m_RenderInstancesBuffer.GetBuffer());
        device.GetD3D11DeviceContext()->CSSetShader(nullptr, nullptr, 0);
        //Sleep(100);
        //device.GetD3D11DeviceContext()->CopyResource(m_IndirectBuffer.GetBuffer(), m_IndirectBuffer2.GetBuffer());
        
    }

    void Render(GraphicsDevice& device, Camera& camera)
    {
        Cull(device, camera);

        size_t offset = 0;
        int instancesOffset = 0;
        for (BaseMesh& baseMesh : m_BaseMeshes)
        {
            Mesh* mesh = baseMesh.mesh;

            mesh->RenderInstanced(device, camera, m_IndirectBuffer, offset, m_InstancesBuffer, instancesOffset);
            instancesOffset += baseMesh.instsCount;
            offset += sizeof(D3D11_DRAW_INSTANCED_INDIRECT_ARGS);
        }
        bool dbg = true;
    }
private:
    std::vector<BaseMesh> m_BaseMeshes;//Mesh*> m_BaseMeshes;
    std::vector<D3D11_DRAW_INSTANCED_INDIRECT_ARGS > m_IndirectArgs;
    std::vector<Instance> m_InstanceData;

    size_t m_DrawCount;

    RWByteAddressBuffer<D3D11_DRAW_INSTANCED_INDIRECT_ARGS > m_IndirectBuffer;
    RWByteAddressBuffer<D3D11_DRAW_INSTANCED_INDIRECT_ARGS> m_IndirectBuffer2;

    StructuredBuffer<Instance> m_RefInstancesBuffer;
    RWStructuredBuffer<Instance> m_RenderInstancesBuffer;
    StructuredBuffer<Instance> m_InstancesBuffer;
    GraphicsConstantsBuffer<CullConsts> m_CullConstantBuffer;

    GraphicsShader m_CullShader;
};