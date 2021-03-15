#pragma once
#include <dxgi.h>
#include <vector>

class DisplayAdaptersList
{
public:
    DisplayAdaptersList();
    ~DisplayAdaptersList();

    IDXGIAdapter* GetAdapter(unsigned int i);

    size_t GetAdaptersCount();
    void LogInfo();
private:
    std::vector<IDXGIAdapter*> m_Adapters;
};