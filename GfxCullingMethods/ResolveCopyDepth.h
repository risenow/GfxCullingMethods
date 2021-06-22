#pragma once
#include "DepthResolveCopyShaderStorage.h"
#include "GraphicsDevice.h"
#include "Texture2D.h"

void ResolveCopyDepth(GraphicsDevice& device, Texture2D& dest, const Texture2D& src);