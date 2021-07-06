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
    GraphicsTextureCollection(GraphicsDevice& device);
    ~GraphicsTextureCollection();

    auto Add(GraphicsDevice& device, const std::string& file, bool isNormalMap = false, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
    Texture2D* RequestTexture(GraphicsDevice& device, const std::string& file, bool isNormalMap = false);// finds texture or creates it if non existant then return
    Texture2D* GetBlackTexture();
    Texture2D* GetBlueTexture();
    Texture2D* GetWhiteTexture();
  private:
      Texture2D InitializeMonotoneTexture(GraphicsDevice& device, uint8_t color[4]);
      void InitializeBlackTexture(GraphicsDevice& device);
      void InitializeBlueTexture(GraphicsDevice& device);
      void InitializeWhiteTexture(GraphicsDevice& device);

      Texture2D m_BlackTexture;
      Texture2D m_BlueTexture;
      Texture2D m_WhiteTexture;

};
