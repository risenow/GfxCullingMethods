#include "DisplayAdaptersList.h"
#include "Log.h"
#include "dxlogicsafety.h"
#include <cstring>

DisplayAdaptersList::DisplayAdaptersList()
{
    IDXGIFactory* pDXGIFactory = nullptr;
    D3D_HR_OP(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pDXGIFactory));
    IDXGIAdapter* currentAdapter = nullptr;
    UINT i = 0;
    while (pDXGIFactory->EnumAdapters(i, &currentAdapter) == S_OK)
    {
        m_Adapters.push_back(currentAdapter);
        i++;
    }
    pDXGIFactory->Release();

    LogInfo();
}
DisplayAdaptersList::~DisplayAdaptersList()
{
    for (int i = 0; i < m_Adapters.size(); i++)
    {
        m_Adapters[i]->Release();
    }
}

IDXGIAdapter* DisplayAdaptersList::GetAdapter(unsigned int i)
{
    assert(i < m_Adapters.size());
    return m_Adapters[i];
}
size_t DisplayAdaptersList::GetAdaptersCount()
{
    return m_Adapters.size();
}

void DisplayAdaptersList::LogInfo()
{
    std::wstring logBuffer;
    LOG_BUFFER(std::string("Adapters enumerated:"), logBuffer);
    for (unsigned int i = 0; i < m_Adapters.size(); i++)
    {
        DXGI_ADAPTER_DESC currentAdapterDesc;
        D3D_HR_OP(m_Adapters[i]->GetDesc(&currentAdapterDesc));
        LOG_BUFFER(std::string("Adapter index: " + std::to_string(i)), logBuffer);
        LOG_BUFFER(wstrtostr_fast(std::wstring(L"Description: ") + std::wstring(currentAdapterDesc.Description)), logBuffer);
        LOG_BUFFER(std::string(""), logBuffer);
    }
	popGetLogger().WriteUsingMode({ &LOG_CONSOLE_STREAM(), &LOG_FILE_STREAM() }, logBuffer, RuntimeLogMode_None);
}

