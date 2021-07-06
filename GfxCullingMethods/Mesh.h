#pragma once
#include <vector>
#include "VertexData.h"
#include "GraphicsBuffer.h"
#include "GraphicsInputLayout.h"
#include "GraphicsShader.h"
#include "AABB.h"
#include "GraphicsTextureCollection.h"
#include "Camera.h"
#include "VertexFormatHelper.h"
#include "GraphicsConstantsBuffer.h"
#include "RenderStatistics.h"
#include "BasicVertexShaderStorage.h"
#include "BasicPixelShaderStorage.h"
#include "basicvsconstants.h"
#include "pathutils.h"
#include "gfxutils.h"
#include "glm/glm.hpp"
#include "tiny_obj_loader.h"

class Mesh
{
public:
    Mesh();

    static GraphicsShader& GetVertexShader(GraphicsDevice& dev);
    static GraphicsShader& GetPixelShader(GraphicsDevice& dev);

    void Initialize();

    static void ReleaseShadersGPUData(GraphicsDevice& device);
    static void FromVertexData(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, VertexData& vertexData, const GraphicsShader& vertexShader, const GraphicsShader& pixelShader, Texture2D* diffuse, Mesh& mesh);
    //TODO(instancing):
    //merge in 1 option
    static void LoadFromFile(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, const std::string& file, std::vector<Mesh*>& meshes, AABB& superAABB);
    
    RenderStatistics Render(GraphicsDevice& device, Camera& camera, const glm::mat4x4& modelMatrix = glm::identity<glm::mat4x4>());
    RenderStatistics RenderInstanced(GraphicsDevice& device, Camera& camera, GraphicsBuffer& argsBuffer, size_t bufferOffset, GraphicsBuffer& instancesBuffer, size_t instancesOffset);

    void Reset(GraphicsDevice& device, VertexData& data, Texture2D* diff, AABB aabb);

    void ReleaseGPUData();

    int GetPrimCount();

    int GetVertexCount();

    AABB GetAABB();
private:
    int m_VertexCount;
    Texture2D* m_Diffuse;
    VertexBuffer m_VertexBuffer;
    GraphicsConstantsBuffer<BasicVSConsts> m_ConstantsBuffer;
    GraphicsShader m_VertexShader;
    GraphicsShader m_PixelShader;
    GraphicsInputLayout m_InputLayout;
    AABB m_AABB;

    bool m_NormalsEnabled;
    bool m_TexCoordsEnabled;
};