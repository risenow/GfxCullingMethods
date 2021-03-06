#include "Mesh.h"

Mesh::Mesh() : m_Diffuse(nullptr), m_VertexCount(0), m_TexCoordsEnabled(false), m_NormalsEnabled(false) {}

GraphicsShader& Mesh::GetVertexShader(GraphicsDevice& dev)
{
    static GraphicsShader s = GraphicsShader::FromFile(dev, GraphicsShaderType_Vertex, L"Data/shaders/basicvs.hlsl");
    return s;
}
GraphicsShader& Mesh::GetPixelShader(GraphicsDevice& dev)
{
    static GraphicsShader s = GraphicsShader::FromFile(dev, GraphicsShaderType_Pixel, L"Data/shaders/basicps.hlsl");
    return s;
}

void Mesh::Initialize() {}

void Mesh::ReleaseShadersGPUData(GraphicsDevice& device)
{
    GetVertexShader(device).ReleaseGPUData();
    GetPixelShader(device).ReleaseGPUData();
}
void Mesh::FromVertexData(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, VertexData& vertexData, const GraphicsShader& vertexShader, const GraphicsShader& pixelShader, Texture2D* diffuse, Mesh& mesh)
{
    Mesh* currMesh = &mesh;

    VertexFormat vertexFormat = vertexData.GetVertexFormat();
    currMesh->m_NormalsEnabled = vertexData.HasNormals();
    currMesh->m_TexCoordsEnabled = vertexData.HasTexCoords();
    currMesh->m_VertexBuffer = VertexBuffer(device, vertexData);
    currMesh->m_VertexShader = BasicVertexShaderStorage::GetInstance().GetShader(BasicVertexShaderStorage::NORMALS_ENABLED | BasicVertexShaderStorage::TEXCOORDS_ENABLED);
    currMesh->m_PixelShader = BasicPixelShaderStorage::GetInstance().GetShader(BasicPixelShaderStorage::NORMALS_ENABLED | BasicPixelShaderStorage::TEXCOORDS_ENABLED);
    currMesh->m_InputLayout = GraphicsInputLayout(device, vertexFormat, currMesh->m_VertexShader);
    currMesh->m_VertexCount = vertexData.GetNumVertexes();
    BasicVSConsts temp;
    currMesh->m_ConstantsBuffer = GraphicsConstantsBuffer<BasicVSConsts>(device, temp);
    assert(currMesh->m_ConstantsBuffer.GetBuffer());

    Texture2D* diff = diffuse;
    currMesh->m_Diffuse = diff != nullptr ? diff : textureCollection.GetWhiteTexture();
}

//TODO(instancing):
//merge in 1 option
void Mesh::LoadFromFile(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, const std::string& file, std::vector<Mesh*>& meshes, AABB& superAABB)
{
    std::string path = ExcludeFileFromPath(file);

    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = ""; // Path to material files

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(file, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    std::vector<Texture2D*> diffTexs;
    diffTexs.reserve(materials.size());

    for (const tinyobj::material_t& mat : materials)
    {
        diffTexs.push_back(mat.diffuse_texname.size() ? textureCollection.RequestTexture(device, path + mat.diffuse_texname) : nullptr);
    }

    for (size_t s = 0; s < shapes.size(); s++)
    {
        const tinyobj::shape_t& shape = shapes[s];
        const std::vector<tinyobj::index_t>& indices = shape.mesh.indices;
        const std::vector<unsigned char>& num_face_verts = shape.mesh.num_face_vertices;

        bool normsEnabled = attrib.normals.size() > 0;
        bool tcEnabled = attrib.texcoords.size() > 0;

        std::vector<VertexPropertyPrototype> vertexPropertyProtos = { VertexFormatHelper::PurePosPropProto(0, 0) };
        if (normsEnabled)
            vertexPropertyProtos.push_back(VertexFormatHelper::PureNormalPropProto(0, 0));
        if (tcEnabled)
            vertexPropertyProtos.push_back(VertexFormatHelper::PureTexCoordPropProto(0, 0));

        //currently supporting only meshes with both tcs and normals
        assert(normsEnabled && tcEnabled);

        Mesh* currMesh = new Mesh();
        currMesh->m_NormalsEnabled = normsEnabled;
        currMesh->m_TexCoordsEnabled = tcEnabled;
        currMesh->m_AABB = AABB();
        meshes.push_back(currMesh);

        VertexFormat vertexFormat(vertexPropertyProtos);
        VertexData vertexData(vertexFormat, shape.mesh.indices.size());
        VertexProperty posDesc = vertexData.GetVertexPropertyByName(VertexDataSemantics_PurePos);
        VertexProperty tcDesc = vertexData.GetVertexPropertyByName(VertexDataSemantics_PureTexCoord);
        VertexProperty normsDesc = vertexData.GetVertexPropertyByName(VertexDataSemantics_PureNormal);

        for (size_t i = 0; i < shape.mesh.num_face_vertices.size(); i++)
        {
            assert(shape.mesh.num_face_vertices[i] == 3);
            for (size_t j = 0; j < 3; j++)
            {
                const tinyobj::index_t& idx = indices[i * 3 + j];

                glm::vec4 pos = glm::vec4(attrib.vertices[idx.vertex_index * 3], attrib.vertices[idx.vertex_index * 3 + 1], attrib.vertices[idx.vertex_index * 3 + 2], 1.0f);
                currMesh->m_AABB.Extend(glm::vec3(pos));
                superAABB.Extend(glm::vec3(pos));
                PullVecToVertexData<glm::vec4>(i * 3 + j, pos, vertexData, posDesc);
                if (normsEnabled)
                {
                    glm::vec3 normal = glm::vec3(attrib.normals[idx.normal_index * 3], attrib.normals[idx.normal_index * 3 + 1], attrib.normals[idx.normal_index * 3 + 2]);
                    PullVecToVertexData<glm::vec3>(i * 3 + j, normal, vertexData, normsDesc);
                }
                if (tcEnabled)
                {
                    glm::vec2 tc = glm::vec2(attrib.texcoords[idx.texcoord_index * 2], attrib.texcoords[idx.texcoord_index * 2 + 1]);
                    PullVecToVertexData<glm::vec2>(i * 3 + j, tc, vertexData, tcDesc);
                }
            }
        }

        currMesh->m_VertexBuffer = VertexBuffer(device, vertexData);
        currMesh->m_VertexShader = BasicVertexShaderStorage::GetInstance().GetShader(BasicVertexShaderStorage::NORMALS_ENABLED | BasicVertexShaderStorage::TEXCOORDS_ENABLED);
        currMesh->m_PixelShader = BasicPixelShaderStorage::GetInstance().GetShader(BasicPixelShaderStorage::NORMALS_ENABLED | BasicPixelShaderStorage::TEXCOORDS_ENABLED);
        currMesh->m_InputLayout = GraphicsInputLayout(device, vertexFormat, currMesh->m_VertexShader);
        currMesh->m_VertexCount = vertexData.GetNumVertexes();
        BasicVSConsts temp;
        currMesh->m_ConstantsBuffer = GraphicsConstantsBuffer<BasicVSConsts>(device, temp);
        assert(currMesh->m_ConstantsBuffer.GetBuffer());

        Texture2D* diff = shape.mesh.material_ids[0] == -1 ? textureCollection.GetWhiteTexture() : diffTexs[shape.mesh.material_ids[0]];
        currMesh->m_Diffuse = diff != nullptr ? diff : textureCollection.GetWhiteTexture();
    }
}


RenderStatistics Mesh::Render(GraphicsDevice& device, Camera& camera, const glm::mat4x4& modelMatrix)
{
    size_t shaderFlags = 0;
    if (m_NormalsEnabled)
        shaderFlags = shaderFlags | BasicVertexShaderStorage::NORMALS_ENABLED;
    if (m_TexCoordsEnabled)
        shaderFlags = shaderFlags | BasicVertexShaderStorage::TEXCOORDS_ENABLED;
    m_VertexShader = BasicVertexShaderStorage::GetInstance().GetShader(shaderFlags);
    m_PixelShader = BasicPixelShaderStorage::GetInstance().GetShader(shaderFlags);
    m_VertexShader.Bind(device);
    m_PixelShader.Bind(device);
    m_InputLayout.Bind(device);
    m_VertexBuffer.Bind(device);

    ID3D11ShaderResourceView* srv = m_Diffuse->GetSRV();
    device.GetD3D11DeviceContext()->PSSetShaderResources(0, 1, &srv);
    ID3D11SamplerState* sampler = GetTrilinearSampler(device);
    device.GetD3D11DeviceContext()->PSSetSamplers(0, 1, &sampler);

    BasicVSConsts consts;
    consts.view = camera.GetViewMatrix();
    consts.projection = camera.GetProjectionMatrix();
    consts.model = modelMatrix;

    GraphicsConstantsBuffer<BasicVSConsts>& constsBuffer = m_ConstantsBuffer;
    constsBuffer.Update(device, consts);
    constsBuffer.Bind(device, GraphicsShaderMask_Vertex | GraphicsShaderMask_Pixel, 0);

    device.GetD3D11DeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    device.GetD3D11DeviceContext()->Draw(m_VertexCount, 0);

    return { GetPrimCount(), 1 };
}
RenderStatistics Mesh::RenderInstanced(GraphicsDevice& device, Camera& camera, GraphicsBuffer& argsBuffer, size_t bufferOffset, GraphicsBuffer& instancesBuffer, size_t instancesOffset)
{
    size_t shaderFlags = BasicVertexShaderStorage::INSTANCED;
    if (m_NormalsEnabled)
        shaderFlags = shaderFlags | BasicVertexShaderStorage::NORMALS_ENABLED;
    if (m_TexCoordsEnabled)
        shaderFlags = shaderFlags | BasicVertexShaderStorage::TEXCOORDS_ENABLED;
    m_VertexShader = BasicVertexShaderStorage::GetInstance().GetShader(shaderFlags);
    m_PixelShader = BasicPixelShaderStorage::GetInstance().GetShader(shaderFlags);
    m_VertexShader.Bind(device);
    m_PixelShader.Bind(device);
    m_InputLayout.Bind(device);
    m_VertexBuffer.Bind(device);

    ID3D11ShaderResourceView* srv = m_Diffuse->GetSRV();
    device.GetD3D11DeviceContext()->PSSetShaderResources(0, 1, &srv);
    ID3D11SamplerState* sampler = GetTrilinearSampler(device);
    device.GetD3D11DeviceContext()->PSSetSamplers(0, 1, &sampler);

    ID3D11ShaderResourceView* instancesSRV = instancesBuffer.GetSRV();
    device.GetD3D11DeviceContext()->VSSetShaderResources(0, 1, &instancesSRV);

    BasicVSConsts consts;
    consts.view = camera.GetViewMatrix();
    consts.projection = camera.GetProjectionMatrix();
    consts.model = glm::identity<glm::mat4x4>();
    consts.instancesOffset = instancesOffset;

    GraphicsConstantsBuffer<BasicVSConsts>& constsBuffer = m_ConstantsBuffer;
    constsBuffer.Update(device, consts);
    constsBuffer.Bind(device, GraphicsShaderMask_Vertex | GraphicsShaderMask_Pixel, 0);

    device.GetD3D11DeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    device.GetD3D11DeviceContext()->DrawInstancedIndirect(argsBuffer.GetBuffer(), bufferOffset);

    instancesSRV = nullptr;
    device.GetD3D11DeviceContext()->VSSetShaderResources(0, 1, &instancesSRV);

    return { 0, 1 };
}

void Mesh::Reset(GraphicsDevice& device, VertexData& data, Texture2D* diff, AABB aabb)
{
    m_AABB = aabb;
    m_Diffuse = diff;
    m_VertexBuffer = VertexBuffer(device, data);
    m_VertexShader = GetVertexShader(device);
    m_PixelShader = GetPixelShader(device);
    m_InputLayout = GraphicsInputLayout(device, data.GetVertexFormat(), m_VertexShader);
    m_VertexCount = data.GetNumVertexes();

    if (!m_ConstantsBuffer.GetBuffer())
    {
        BasicVSConsts temp;
        m_ConstantsBuffer = GraphicsConstantsBuffer<BasicVSConsts>(device, temp);
    }
}

void Mesh::ReleaseGPUData()
{
    m_ConstantsBuffer.ReleaseGPUData();
    m_VertexBuffer.ReleaseGPUData();
    m_InputLayout.ReleaseGPUData();
}

int Mesh::GetPrimCount()
{
    return m_VertexCount / 3;
}

int Mesh::GetVertexCount()
{
    return m_VertexCount;
}

AABB Mesh::GetAABB()
{
    return m_AABB;
}