#pragma once
#include "glm/glm.hpp"
#include "GraphicsDevice.h"
#include "SuperMeshInstance.h"
#include "DrawInstancedIndirectArgs.h"
#include "GraphicsBuffer.h"
#include "GraphicsConstantsBuffer.h"
#include "ResolveCopyDepth.h"
#include "CullConsts.h"
#include "GenHiZConsts.h"
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
    GPUDrivenRenderer(GraphicsDevice& device) : m_HiZCullingSRV(nullptr)
    {
        m_CullShader = GraphicsShader::FromFile(device, GraphicsShaderType_Compute, L"cullcs.hlsl");
        m_HiZGenShader = GraphicsShader::FromFile(device, GraphicsShaderType_Compute, L"genhizscs.hlsl");

        CullConsts constsCull;
        ZeroMemory(&constsCull, sizeof(constsCull));
        m_CullConstantBuffer = GraphicsConstantsBuffer<CullConsts>(device, constsCull);

        HiZConsts constsHiZ;
        ZeroMemory(&constsHiZ, sizeof(constsHiZ));
        m_HiZConstantsBuffer = GraphicsConstantsBuffer<HiZConsts>(device, constsHiZ);
    }

    void Consume(GraphicsDevice& device, const std::vector<SuperMeshInstance>& insts, const std::vector<SuperMeshInstance>& occInsts)
    {
        m_Occluders = occInsts;

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

        m_CleanIndirectBuffer = RWByteAddressBuffer<D3D11_DRAW_INSTANCED_INDIRECT_ARGS>(device, m_IndirectArgs, GraphicsBuffer::MiscFlag_DrawIndirectArgs);
        m_IndirectBuffer = RWByteAddressBuffer<D3D11_DRAW_INSTANCED_INDIRECT_ARGS>(device, m_IndirectArgs, GraphicsBuffer::MiscFlag_DrawIndirectArgs);
        m_RefInstancesBuffer = StructuredBuffer<Instance>(device, m_InstanceData);
        m_RenderInstancesBuffer = RWStructuredBuffer<Instance>(device, m_InstanceData);
    }
    void OnFrameBegin(GraphicsDevice& device)
    {
        device.GetD3D11DeviceContext()->CopyResource(m_IndirectBuffer.GetBuffer(), m_CleanIndirectBuffer.GetBuffer());
    }


    void Render(GraphicsDevice& device, Camera& camera, ColorSurface& colorTarget, DepthSurface& depthTarget)
    {
        ID3D11RenderTargetView* colorTargetView = colorTarget.GetView();
        ID3D11DepthStencilView* depthView = depthTarget.GetView();
        device.GetD3D11DeviceContext()->OMSetRenderTargets(1, &colorTargetView, depthView);

        if (true)
        {
            glm::vec4 clearColor = glm::vec4(0.0, 0.0, 0.0, 1.0);
            device.GetD3D11DeviceContext()->ClearRenderTargetView(colorTargetView, (float*)&clearColor);
            device.GetD3D11DeviceContext()->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        }

        for (SuperMeshInstance& occ : m_Occluders)
            occ.Render(device, camera);

        ID3D11RenderTargetView* view[] = { nullptr };
        device.GetD3D11DeviceContext()->OMSetRenderTargets(1, view, nullptr);

        GenerateHiZ(device, *depthTarget.GetTexture());
        Cull(device, camera);

        device.GetD3D11DeviceContext()->OMSetRenderTargets(1, &colorTargetView, depthView);

        size_t offset = 0;
        int instancesOffset = 0;
        for (BaseMesh& baseMesh : m_BaseMeshes)
        {
            Mesh* mesh = baseMesh.mesh;

            mesh->RenderInstanced(device, camera, m_IndirectBuffer, offset, m_RenderInstancesBuffer, instancesOffset);
            instancesOffset += baseMesh.instsCount;
            offset += sizeof(D3D11_DRAW_INSTANCED_INDIRECT_ARGS);
        }
    }
//private:
    void ReinitHiZTexture(GraphicsDevice& device, const Texture2D& depth)
    {
        if (m_HiZCullingSRV)
            m_HiZCullingSRV->Release();
        
        for (size_t mip = 0; mip < m_MipsSRV.size(); mip++)
        {
            if (m_MipsSRV[mip])
                m_MipsSRV[mip]->Release();
            if (m_MipsUAV[mip])
                m_MipsUAV[mip]->Release();
            m_MipsSRV[mip] = nullptr;
            m_MipsUAV[mip] = nullptr;
        }
        m_HiZ.ReleaseGPUData();

        DXGI_SAMPLE_DESC sampleDesc = { 1, 0 };

        size_t mipCnt = log(std::min(depth.GetWidth(), depth.GetHeight())) / log(2) + 1;
        m_HiZ = Texture2D(device, depth.GetWidth(), depth.GetHeight(), mipCnt, 1, DXGI_FORMAT_R32_FLOAT, sampleDesc, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, 0, 0);
        m_MipsUAV.resize(mipCnt);
        m_MipsSRV.resize(mipCnt);

        InitMipsShaderResources(device);
    }

    void GenerateHiZ(GraphicsDevice& device, const Texture2D& depth)
    {
        if (!m_HiZ.IsValid())
        {
            ReinitHiZTexture(device, depth);
        }

        if (m_HiZ.GetWidth() != depth.GetWidth() || m_HiZ.GetHeight() != depth.GetHeight())
        {
            ReinitHiZTexture(device, depth);
        }

        ResolveCopyDepth(device, m_HiZ, depth);

        size_t mipsLeft = m_MipsUAV.size() - 1;
        for (size_t mip = 0; mip < m_MipsUAV.size() - 1; )
        {
            size_t mipsToGen = std::min(mipsLeft, (size_t)1u);//2

            HiZConsts consts;
            consts.numMips = mipsToGen;
            consts.srcMipLvl = mip;
            consts.srcMipWidth = m_HiZ.GetWidth() >> mip;
            consts.srcMipHeight = m_HiZ.GetHeight() >> mip;

            m_HiZConstantsBuffer.Update(device, consts);

            m_HiZGenShader.Bind(device);
            m_HiZConstantsBuffer.Bind(device, GraphicsShaderMask_Compute);

            ID3D11ShaderResourceView* mip0 = m_MipsSRV[mip];
            ID3D11UnorderedAccessView* uavs[4];
            for (size_t i = 0; i < mipsToGen; i++) 
            {
                uavs[i] = m_MipsUAV[mip + 1 + i];
            }

            device.GetD3D11DeviceContext()->CSSetShaderResources(0, 1, &mip0);
            device.GetD3D11DeviceContext()->CSSetUnorderedAccessViews(0, mipsToGen, uavs, nullptr);

            device.GetD3D11DeviceContext()->Dispatch(ceil((float)(std::max(m_HiZ.GetWidth() >> (mip + 1), (size_t)1)) / 8.0f), ceil((float)(std::max(m_HiZ.GetHeight() >> (mip + 1), (size_t)1)) / 8.0f), 1);

            mip += mipsToGen;
            mipsLeft -= mipsToGen;

            ZeroMemory(uavs, sizeof(uavs));
            mip0 = nullptr;
            device.GetD3D11DeviceContext()->CSSetShaderResources(0, 1, &mip0);
            device.GetD3D11DeviceContext()->CSSetUnorderedAccessViews(0, mipsToGen, uavs, nullptr);
        }
        
    }

    void InitMipsShaderResources(GraphicsDevice& device)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Texture2D.MipLevels = m_HiZ.GetMipsCount();
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        device.GetD3D11Device()->CreateShaderResourceView((ID3D11Texture2D*)(m_HiZ.GetD3D11Texture2D()), (D3D11_SHADER_RESOURCE_VIEW_DESC*)&srvDesc, (ID3D11ShaderResourceView**)&m_HiZCullingSRV);

        for (size_t mip = 0; mip < m_HiZ.GetMipsCount(); mip++)
        {
            D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
            uavDesc.Texture2D.MipSlice = mip;
            uavDesc.Format = DXGI_FORMAT_R32_FLOAT;
            uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
            device.GetD3D11Device()->CreateUnorderedAccessView((ID3D11Texture2D*)(m_HiZ.GetD3D11Texture2D()), (D3D11_UNORDERED_ACCESS_VIEW_DESC*)&uavDesc, (ID3D11UnorderedAccessView**)&m_MipsUAV[mip]);

            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
            srvDesc.Texture2D.MipLevels = 1;
            srvDesc.Texture2D.MostDetailedMip = mip;
            srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            device.GetD3D11Device()->CreateShaderResourceView((ID3D11Texture2D*)(m_HiZ.GetD3D11Texture2D()), (D3D11_SHADER_RESOURCE_VIEW_DESC*)&srvDesc, (ID3D11ShaderResourceView**)&m_MipsSRV[mip]);
        }
    }

    void Cull(GraphicsDevice& device, Camera& camera)
    {
        Camera::Frustum fr = Camera::Frustum(camera);

        CullConsts consts;
        consts.drawCount = m_DrawCount;
        consts.frLeft = fr.planes[0].AsVecNormalized();
        consts.frRight = fr.planes[1].AsVecNormalized();
        consts.frTop = fr.planes[2].AsVecNormalized();
        consts.frBottom = fr.planes[3].AsVecNormalized();
        consts.rightDir = glm::vec4(camera.GetRightVec(), 1.0f);
        consts.topDir = glm::vec4(camera.GetTopVec(), 1.0f);
        consts.camPos = glm::vec4(camera.GetPosition(), 1.0f);
        consts.viewProj = camera.GetViewProjectionMatrix();
        consts.view = camera.GetViewMatrix();
        consts.proj = camera.GetProjectionMatrix();

        m_CullConstantBuffer.Update(device, consts);
        m_CullConstantBuffer.Bind(device, GraphicsShaderMask_Compute);

        ID3D11ShaderResourceView* srvs[] = { m_RefInstancesBuffer.GetSRV(), m_HiZCullingSRV };
        ID3D11UnorderedAccessView* uavs[] = { m_RenderInstancesBuffer.GetUAV(), m_IndirectBuffer.GetUAV() };

        unsigned int counts[2] = { 0, 0 };
        device.GetD3D11DeviceContext()->CSSetShaderResources(0, 2, srvs);
        device.GetD3D11DeviceContext()->CSSetUnorderedAccessViews(0, 2, uavs, nullptr);

        m_CullShader.Bind(device);

        device.GetD3D11DeviceContext()->Dispatch(ceil((float)m_InstanceData.size() / 256.0f), 1, 1);

        ZeroMemory(srvs, sizeof(srvs));
        ZeroMemory(uavs, sizeof(uavs));
        device.GetD3D11DeviceContext()->CSSetShaderResources(0, 2, srvs);
        device.GetD3D11DeviceContext()->CSSetUnorderedAccessViews(0, 2, uavs, nullptr);
        device.GetD3D11DeviceContext()->CSSetShader(nullptr, nullptr, 0);
    }

    //void Copy

    std::vector<SuperMeshInstance> m_Occluders;

    std::vector<BaseMesh> m_BaseMeshes;
    std::vector<D3D11_DRAW_INSTANCED_INDIRECT_ARGS > m_IndirectArgs;
    std::vector<Instance> m_InstanceData;

    size_t m_DrawCount;

    RWByteAddressBuffer<D3D11_DRAW_INSTANCED_INDIRECT_ARGS > m_IndirectBuffer;
    RWByteAddressBuffer<D3D11_DRAW_INSTANCED_INDIRECT_ARGS> m_CleanIndirectBuffer;

    StructuredBuffer<Instance> m_RefInstancesBuffer;
    RWStructuredBuffer<Instance> m_RenderInstancesBuffer;
    StructuredBuffer<Instance> m_InstancesBuffer;
    GraphicsConstantsBuffer<CullConsts> m_CullConstantBuffer;

    Texture2D m_HiZ;
    GraphicsConstantsBuffer<HiZConsts> m_HiZConstantsBuffer;
    std::vector<ID3D11UnorderedAccessView*> m_MipsUAV;
    std::vector<ID3D11ShaderResourceView*> m_MipsSRV;
    ID3D11ShaderResourceView* m_HiZCullingSRV;

    GraphicsShader m_HiZGenShader;
    GraphicsShader m_CullShader;
    GraphicsShader m_CopyDepth;
};