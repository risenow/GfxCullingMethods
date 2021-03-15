#pragma once
#include <Windows.h>
#include "winerrorcodesinterpreteutil.h"
#include "Log.h"
#include <assert.h>

#define ENABLE_ASSERTS

#if defined(_DEBUG) && defined(ENABLE_ASSERTS)
#define popAssert(op) assert(op)
#else
#define popAssert(op)
#endif

#define WIN_HR_OP(op) { HRESULT hr = op; if (hr != S_OK){ LOG(std::string("Operation error: " + WinHRtoStr(hr) + " Line: " + std::to_string(__LINE__))); popAssert(false); }}