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

struct Backet
{
    
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
                    m_BaseMeshes.push_back(mesh);
                instancesPerMesh[mesh].push_back({ 0, superInst.GetTransform(), mesh->GetAABB().GetSBB() });
            }
        }

        size_t currentInstanceStart = 0;
        size_t batchIndex = 0;
        for (Mesh* mesh : m_BaseMeshes)
        {
            std::vector<Instance>& instances = instancesPerMesh[mesh];

            m_DrawCount += instances.size();

            DrawInstancedIndirectArgs args;
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

            m_RefInstancesBuffer = StructuredBuffer<Instance>(device, m_InstanceData);
            m_RenderInstancesBuffer = RWStructuredBuffer<Instance>(device, m_InstanceData);
            m_IndirectBuffer = RWByteAddressBuffer<D3D11_DRAW_INSTANCED_INDIRECT_ARGS>(device, m_IndirectArgs, GraphicsBuffer::MiscFlag_DrawIndirectArgs);
        }
    }
    void OnFrameBegin(GraphicsDevice& device)
    {
        device.GetD3D11DeviceContext()->UpdateSubresource((ID3D11Resource*)m_IndirectBuffer.GetBuffer(), 0, nullptr, m_IndirectArgs.data(), sizeof(D3D11_DRAW_INSTANCED_INDIRECT_ARGS) * m_IndirectArgs.size(), 0);
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

        m_CullShader.Bind(device);
        
        ID3D11ShaderResourceView* refInstancesSRV = m_RefInstancesBuffer.GetSRV();
        ID3D11UnorderedAccessView* uavs[] = { m_RenderInstancesBuffer.GetUAV(), m_IndirectBuffer.GetUAV() };

        device.GetD3D11DeviceContext()->CSSetShaderResources(0, 1, &refInstancesSRV);
        device.GetD3D11DeviceContext()->CSGetUnorderedAccessViews(0, 2, uavs);

        device.GetD3D11DeviceContext()->Dispatch(ceil((float)m_InstanceData.size() / 256.0f), 1, 1);
    }

    void Render(GraphicsDevice& device, Camera& camera)
    {
        Cull(device);

        size_t offset = 0;
        for (Mesh* mesh : m_BaseMeshes)
        {
            mesh->RenderInstanced(device, camera, m_IndirectBuffer, offset, m_RenderInstancesBuffer);
            offset += sizeof(D3D11_DRAW_INSTANCED_INDIRECT_ARGS);
        }
    }
private:
    std::vector<Mesh*> m_BaseMeshes;
    std::vector<D3D11_DRAW_INSTANCED_INDIRECT_ARGS > m_IndirectArgs;
    std::vector<Instance> m_InstanceData;

    size_t m_DrawCount;

    RWByteAddressBuffer<D3D11_DRAW_INSTANCED_INDIRECT_ARGS > m_IndirectBuffer;
    StructuredBuffer<Instance> m_RefInstancesBuffer;
    RWStructuredBuffer<Instance> m_RenderInstancesBuffer;
    GraphicsConstantsBuffer<CullConsts> m_CullConstantBuffer;

    GraphicsShader m_CullShader;
};