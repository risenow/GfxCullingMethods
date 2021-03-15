#include "GraphicsTextureCollection.h"
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb/stb_image.h"
#include "WICTextureLoader11.h"
#include "DDSTextureLoader11.h"
#include <algorithm>
#include <string>

bool IsFileOfSupported2DFormat(const std::string& file, std::string& format)
{
    if (!file.size())
        return false;
    format.reserve(6);
    for (int i = file.size() - 1; (i >= 0) && (file[i] != '.'); i--)
        format.push_back(file[i]);
    std::reverse(format.begin(), format.end());

    bool supported = false;
    if (format == "jpg" || format == "JPG" || format == "png" || format == "bmp")
        supported = true;

    return supported;
}

Texture2D* GraphicsTextureCollection::RequestTexture(GraphicsDevice& device, const std::string& file, bool isNormalMap) //mb return shared ptr?
{
    auto it = find(file);
    if (it == end())
    {
        auto it_ = Add(device, file, isNormalMap);
        if (it_ != end())
            return (*it_).second.get();
    }
    else
        return it->second.get();
    return nullptr;
}
