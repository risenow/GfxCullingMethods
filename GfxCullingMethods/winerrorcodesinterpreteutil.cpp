#include "winerrorcodesinterpreteutil.h"

std::string WinHRtoStr(HRESULT hr)
{
    switch (hr)
    {
    case S_OK:
        return "Operation succesful";
        break;
    case S_FALSE:
        return "Alternate success value, indicating a successful but nonstandard completion (the precise meaning depends on context)";
        break;
    case E_ABORT:
        return "Operation aborted";
        break;
    case E_ACCESSDENIED:
        return "General access denied error";
        break;
    case E_FAIL:
        return "Unspecified failure";
        break;
    case E_HANDLE:
        return "Handle that is not valid";
        break;
    case E_INVALIDARG:
        return "One or more arguments are not valid";
        break;
    case E_NOINTERFACE:
        return "No such interface supported";
        break;
    case E_NOTIMPL:
        return "Not implemented";
        break;
    case E_OUTOFMEMORY:
        return "Failed to allocate necessary memory";
        break;
    case E_POINTER:
        return "Pointer that is not valid";
        break;
    case E_UNEXPECTED:
        return "Unexpected failure";
        break;
    }

    return "Invalid hresult value";
}