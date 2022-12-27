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
#include "GraphicsSurface.h"
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
    GPUDrivenRenderer(GraphicsDevice& device, bool cullSmallObjects, float smallObjectSizeThreshold);

    void Consume(GraphicsDevice& device, const std::vector<SuperMeshInstance>& insts, const std::vector<SuperMeshInstance>& occInsts);
    void OnFrameBegin(GraphicsDevice& device);

    void Render(GraphicsDevice& device, Camera& camera, ColorSurface& colorTarget, DepthSurface& depthTarget, bool useOldCullingResults = false);
private:
    void ReinitHiZTexture(GraphicsDevice& device, const Texture2D& depth);

    void GenerateHiZ(GraphicsDevice& device, const Texture2D& depth);

    void InitMipsShaderResources(GraphicsDevice& device);
    void Cull(GraphicsDevice& device, Camera& camera);

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