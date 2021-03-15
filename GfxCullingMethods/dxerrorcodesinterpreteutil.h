#pragma once
#include <string>
#include <Windows.h>

bool IsDXGIStatus(HRESULT hr);

std::string WinHRtoStr(HRESULT hr);
std::string D3DHRtoStr(HRESULT hr);
