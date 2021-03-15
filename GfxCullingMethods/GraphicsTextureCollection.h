#pragma once
#include "Texture2D.h"
#include "GraphicsDevice.h"
#include "multisampleutils.h"
#include <map>
#include <memory>
#include "helperutil.h"
#include "strutils.h"
#include "WICTextureLoader11.h"
#include "DDSTextureLoader11.h"
#include "DirectXTex.h"
#include <algorithm>
#include <math.h>

bool IsFileOfSupported2DFormat(const std::string& file, std::string& format);

class  GraphicsTextureCollection : private std::map<std::string, std::shared_ptr<Texture2D>>
{
public:
    GraphicsTextureCollection(GraphicsDevice& device) 
    {
        InitializeWhiteTexture(device);
        InitializeBlackTexture(device);
        InitializeBlueTexture(device);
    }
    ~GraphicsTextureCollection() {}

    auto Add(GraphicsDevice& device, const std::string& file, bool isNormalMap = false, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB)
    {
        std::string sformat;
        if (IsFileOfSupported2DFormat(file, sformat))
        {
            ID3D11Resource* res;
            ID3D11ShaderResourceView* srv;
            HRESULT hr = DirectX::CreateWICTextureFromFile(device.GetD3D11Device(), strtowstr_fast(file).c_str(), (ID3D11Resource**)& res, &srv);

            if (SUCCEEDED(hr))
            {
                auto i = insert({ file, std::make_shared<Texture2D>(device, (ID3D11Texture2D*)res, srv) });
                return i.first;
            }

            return end();
        }

        if (sformat == "dds")
        {
            ID3D11Resource* res;
            ID3D11ShaderResourceView* srv;
            HRESULT hr = DirectX::CreateDDSTextureFromFile(
                device.GetD3D11Device(),
                strtowstr_fast(file).c_str(),
                (ID3D11Resource * *)& res,
                &srv);

            if (SUCCEEDED(hr))
            {
                auto i = insert({ file, std::make_shared<Texture2D>(device, (ID3D11Texture2D*)res, srv) });
                return i.first;
            }
            return end();
        }

        if (sformat == "tga")
        {
            ID3D11Resource* res = nullptr;
            DirectX::ScratchImage scrImage;
            DirectX::TexMetadata texMetaData;
            DirectX::ScratchImage mipChain;
            DirectX::TexMetadata mipChainMetaData;

            HRESULT hr = ( DirectX::LoadFromTGAFile(strtowstr_fast(file).c_str(), &texMetaData, scrImage));

            int levels = 0; //mb use lower levels num for normal maps

            HRESULT hr2 = -1;
            if (scrImage.GetImageCount())
            {
                DirectX::GenerateMipMaps(scrImage.GetImages()[0], DirectX::TEX_FILTER_DEFAULT, levels, mipChain, false);
                hr2 = (DirectX::CreateTextureEx(device.GetD3D11Device(), mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, !isNormalMap, &res));
            }

            if (SUCCEEDED(hr) && SUCCEEDED(hr2))
            {
                auto i = insert({ file, std::make_shared<Texture2D>(device, (ID3D11Texture2D*)res, nullptr) });
                return i.first;
            }
        }

        return end();
    }
    Texture2D* RequestTexture(GraphicsDevice& device, const std::string& file, bool isNormalMap = false);// finds texture or creates it if non existant then return
    Texture2D* GetBlackTexture()
    {
        return &m_BlackTexture;
    }
    Texture2D* GetBlueTexture()
    {
        return &m_BlueTexture;
    }
    Texture2D* GetWhiteTexture()
    {
        return &m_WhiteTexture;
    }
  private:
      Texture2D InitializeMonotoneTexture(GraphicsDevice& device, uint8_t color[4])
      {
          D3D11_SUBRESOURCE_DATA data;
          data.pSysMem = (void*)color;
          data.SysMemPitch = sizeof(uint8_t) * 4;
          data.SysMemSlicePitch = 0;
          return Texture2D(device, 1, 1, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, GetDefaultSampleDesc(), D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, &data);
      }
      void InitializeBlackTexture(GraphicsDevice& device)
      {
          uint8_t p[4] = { 0, 0, 0, 255 };
          m_BlackTexture = InitializeMonotoneTexture(device, p);
      }
      void InitializeBlueTexture(GraphicsDevice& device)
      {
          uint8_t p[4] = { 0, 0, 255, 255 };
          m_BlueTexture = InitializeMonotoneTexture(device, p);
      }
      void InitializeWhiteTexture(GraphicsDevice& device)
      {
          uint8_t p[4] = { 255, 255, 255, 255 };
          m_WhiteTexture = InitializeMonotoneTexture(device, p);
      }

      Texture2D m_BlackTexture;
      Texture2D m_BlueTexture;
      Texture2D m_WhiteTexture;

};
