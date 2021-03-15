#pragma once
#include <d3d11.h>
#include "MultisampleType.h"
#include "GraphicsDevice.h"

unsigned int GetMultisampleQualityNum(GraphicsDevice& device, DXGI_FORMAT format, MultisampleType multisample);
DXGI_SAMPLE_DESC GetSampleDesc(GraphicsDevice& device, DXGI_FORMAT format, MultisampleType);
DXGI_SAMPLE_DESC GetDefaultSampleDesc();