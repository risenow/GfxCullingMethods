#pragma once
#include "VertexData.h"
#include "glm/glm.hpp"
#include "typesalias.h"

class VertexFormatHelper
{
public:
    static VertexPropertyPrototype PurePosPropProto(index_t slotIndex, index_t semanticIndex = 0)
    {
        return CreateVertexPropertyPrototype<glm::vec4>(VertexDataSemantics_PurePos, semanticIndex, slotIndex);
    }
    static VertexPropertyPrototype PureColorPropProto(index_t slotIndex, index_t semanticIndex = 0)
    {
        return CreateVertexPropertyPrototype<glm::vec4>(VertexDataSemantics_PureColor, semanticIndex, slotIndex);
    }
    static VertexPropertyPrototype PureNormalPropProto(index_t slotIndex, index_t semanticIndex = 0)
    {
        return CreateVertexPropertyPrototype<glm::vec3>(VertexDataSemantics_PureNormal, semanticIndex, slotIndex);
    }
    static VertexPropertyPrototype PureTexCoordPropProto(index_t slotIndex, index_t semanticIndex = 0)
    {
        return CreateVertexPropertyPrototype<glm::vec2>(VertexDataSemantics_PureTexCoord, semanticIndex, slotIndex);
    }
};