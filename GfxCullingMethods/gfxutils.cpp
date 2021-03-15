#include "gfxutils.h"
#include "SuperMesh.h"
#include <assert.h>

ID3D11SamplerState* CreateSampler(GraphicsDevice& device, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE uMode,
                                  D3D11_TEXTURE_ADDRESS_MODE vMode, D3D11_TEXTURE_ADDRESS_MODE wMode,
                                  float mipLODBias, UINT maxAnisotropy, D3D11_COMPARISON_FUNC compFunc,
                                  const glm::vec4& borderCol, float minLod, float maxLod)
{
    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = filter;
    samplerDesc.AddressU = uMode;
    samplerDesc.AddressV = vMode;
    samplerDesc.AddressW = wMode;
    samplerDesc.MipLODBias = mipLODBias;
    samplerDesc.MaxAnisotropy = maxAnisotropy;
    samplerDesc.ComparisonFunc = compFunc;
    *(glm::vec4*)samplerDesc.BorderColor = borderCol;
    samplerDesc.MinLOD = minLod;
    samplerDesc.MaxLOD = maxLod;

    ID3D11SamplerState* sampler;
    device.GetD3D11Device()->CreateSamplerState(&samplerDesc, &sampler);

    return sampler;
}
ID3D11SamplerState* GetTrilinearSampler(GraphicsDevice& device, bool sep)
{
    static ID3D11SamplerState* sampler = CreateSampler(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR,
                                                        D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP,
                                                        D3D11_TEXTURE_ADDRESS_WRAP, 0.0f, 1, D3D11_COMPARISON_ALWAYS,
                                                        glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, D3D11_FLOAT32_MAX);
    return !sep ? sampler : CreateSampler(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR,
                                          D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP,
                                          D3D11_TEXTURE_ADDRESS_WRAP, 0.0f, 1, D3D11_COMPARISON_ALWAYS,
                                          glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, D3D11_FLOAT32_MAX);;
}
SuperMesh* CreatePawnPlaceholderMesh(GraphicsDevice& device, GraphicsTextureCollection& textureCollection)
{
    Mesh* mesh = new Mesh();
    SuperMesh* superMesh = new SuperMesh({ mesh });
    VertexFormat vertexFormat({ CreateVertexPropertyPrototype<glm::vec4>(VertexDataSemantics_PurePos, 0, 0),
                                CreateVertexPropertyPrototype<glm::vec3>(VertexDataSemantics_PureNormal, 0, 0),
                                CreateVertexPropertyPrototype<glm::vec2>(VertexDataSemantics_PureTexCoord, 0, 0) });
    VertexData vertexData(vertexFormat, 12);
    VertexProperty posDesc = vertexData.GetVertexPropertyByName(VertexDataSemantics_PurePos);
    VertexProperty normalDesc = vertexData.GetVertexPropertyByName(VertexDataSemantics_PureNormal);
    VertexProperty tcDesc = vertexData.GetVertexPropertyByName(VertexDataSemantics_PureTexCoord);

    PullVecToVertexData<glm::vec4>(0, glm::vec4(-0.5, 0.0, 1.0, 1.0), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(0, glm::vec3(0.0, 1.0, 0.0), vertexData, normalDesc);
    PullVecToVertexData<glm::vec2>(0, glm::vec2(1.0, 0.0), vertexData, tcDesc); //

    PullVecToVertexData<glm::vec4>(1, glm::vec4(0.5, 0.0, 1.0, 1.0), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(1, glm::vec3(0.0, 1.0, 0.0), vertexData, normalDesc);
    PullVecToVertexData<glm::vec2>(1, glm::vec2(1.0, 0.0), vertexData, tcDesc);//

    PullVecToVertexData<glm::vec4>(2, glm::vec4(-0.5, 0.0, 0.0, 1.0), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(2, glm::vec3(0.0, 1.0, 0.0), vertexData, normalDesc);
    PullVecToVertexData<glm::vec2>(2, glm::vec2(1.0, 0.0), vertexData, tcDesc);//

    //
    PullVecToVertexData<glm::vec4>(3, glm::vec4(-0.5, 0.0, 0.0, 1.0), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(3, glm::vec3(0.0, 1.0, 0.0), vertexData, normalDesc);
    PullVecToVertexData<glm::vec2>(3, glm::vec2(1.0, 0.0), vertexData, tcDesc); //

    PullVecToVertexData<glm::vec4>(4, glm::vec4(0.5, 0.0, 0.0, 1.0), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(4, glm::vec3(0.0, 1.0, 0.0), vertexData, normalDesc);
    PullVecToVertexData<glm::vec2>(4, glm::vec2(1.0, 0.0), vertexData, tcDesc);//

    PullVecToVertexData<glm::vec4>(5, glm::vec4(0.5, 0.0, 1.0, 1.0), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(5, glm::vec3(0.0, 1.0, 0.0), vertexData, normalDesc);
    PullVecToVertexData<glm::vec2>(5, glm::vec2(1.0, 0.0), vertexData, tcDesc);//

    //
    PullVecToVertexData<glm::vec4>(6, glm::vec4(-1.0, 0.0, 1.0, 1.0), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(6, glm::vec3(0.0, 1.0, 0.0), vertexData, normalDesc);
    PullVecToVertexData<glm::vec2>(6, glm::vec2(1.0, 0.0), vertexData, tcDesc); //

    PullVecToVertexData<glm::vec4>(7, glm::vec4(0.0, 0.0, 1.0, 1.0), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(7, glm::vec3(0.0, 1.0, 0.0), vertexData, normalDesc);
    PullVecToVertexData<glm::vec2>(7, glm::vec2(1.0, 0.0), vertexData, tcDesc);//

    PullVecToVertexData<glm::vec4>(8, glm::vec4(0.0, 0.0, 1.5, 1.0), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(8, glm::vec3(0.0, 1.0, 0.0), vertexData, normalDesc);
    PullVecToVertexData<glm::vec2>(8, glm::vec2(1.0, 0.0), vertexData, tcDesc);//

    //
    PullVecToVertexData<glm::vec4>(9, glm::vec4(0.0, 0.0, 1.5, 1.0), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(9, glm::vec3(0.0, 1.0, 0.0), vertexData, normalDesc);
    PullVecToVertexData<glm::vec2>(9, glm::vec2(1.0, 0.0), vertexData, tcDesc); //

    PullVecToVertexData<glm::vec4>(10, glm::vec4(0.0, 0.0, 1.0, 1.0), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(10, glm::vec3(0.0, 1.0, 0.0), vertexData, normalDesc);
    PullVecToVertexData<glm::vec2>(10, glm::vec2(1.0, 0.0), vertexData, tcDesc);//

    PullVecToVertexData<glm::vec4>(11, glm::vec4(1.0, 0.0, 1.0, 1.0), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(11, glm::vec3(0.0, 1.0, 0.0), vertexData, normalDesc);
    PullVecToVertexData<glm::vec2>(11, glm::vec2(1.0, 0.0), vertexData, tcDesc);//

    mesh->Reset(device, vertexData, textureCollection.GetBlueTexture(), AABB());
    return superMesh;
}

SuperMesh* GetPawnPlaceholderMesh(GraphicsDevice& device, GraphicsTextureCollection& textureCollection)
{
    static SuperMesh* mesh = CreatePawnPlaceholderMesh(device, textureCollection);

    return mesh;
}

glm::vec3 GetAxis(size_t i)
{
    assert(i < 3);
    static const glm::vec3 axes[3] = { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) };

    return axes[i];
}