#pragma once
#include <string>
#include "GraphicsDevice.h"

class GraphicsMarker
{
public:
    GraphicsMarker(GraphicsDevice& device, const std::wstring& name) : m_Annotation(nullptr)
    {
       device.GetD3D11DeviceContext()->QueryInterface(__uuidof(ID3DUserDefinedAnnotation), (void**)&m_Annotation);

        if (m_Annotation)
            m_Annotation->BeginEvent(name.c_str());
    }
    ~GraphicsMarker()
    {
        if (m_Annotation)
            m_Annotation->EndEvent();
    }
private:
    ID3DUserDefinedAnnotation* m_Annotation;
};

#define MARKER_BEGIN(device, name) { GraphicsMarker marker( device, name);
#define MARKER_END() }