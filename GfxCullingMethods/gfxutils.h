#pragma once
#include "GraphicsDevice.h"
#include "GraphicsConstantsBuffer.h"
#include "GraphicsTextureCollection.h"
#include "glm/glm.hpp"

ID3D11SamplerState* CreateSampler(GraphicsDevice& device, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE uMode,
                                  D3D11_TEXTURE_ADDRESS_MODE vMode, D3D11_TEXTURE_ADDRESS_MODE wMode,
                                  float mipLODBias, UINT maxAnisotropy, D3D11_COMPARISON_FUNC compFunc,
                                  const glm::vec4& borderCol, float minLod, float maxLod);
ID3D11SamplerState* GetTrilinearSampler(GraphicsDevice& device, bool sep = false);
template<class T>
GraphicsConstantsBuffer<T>& GetConstantsBuffer(GraphicsDevice& device)
{
    static T temp;
    static GraphicsConstantsBuffer<T> constsBuffer(device, temp);
    return constsBuffer;
}
class SuperMesh;
SuperMesh* CreatePawnPlaceholderMesh(GraphicsDevice& device, GraphicsTextureCollection& textureCollection);
SuperMesh* GetPawnPlaceholderMesh(GraphicsDevice& device, GraphicsTextureCollection& textureCollection);
glm::vec3 GetAxis(size_t i);