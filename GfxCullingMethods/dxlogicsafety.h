#pragma once
#include <Windows.h>
#include "dxerrorcodesinterpreteutil.h"
#include "Log.h"
#include <assert.h>

#define D3D_HR_OP(op) { HRESULT hr = op; if (hr != S_OK){ LOG(std::string("Operation error: " + D3DHRtoStr(hr) + " Line: " + std::to_string(__LINE__) + " File: " + (__FILE__))); /*if (!IsDXGIStatus(hr)) popAssert(false);*/ }}