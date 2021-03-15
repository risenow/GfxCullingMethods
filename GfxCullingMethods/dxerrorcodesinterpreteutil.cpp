#include "dxerrorcodesinterpreteutil.h"



bool IsDXGIStatus(HRESULT hr)
{
    return (hr == DXGI_STATUS_OCCLUDED) || (hr == DXGI_STATUS_MODE_CHANGED) || (hr == DXGI_STATUS_MODE_CHANGE_IN_PROGRESS);
}

std::string D3DHRtoStr(HRESULT hr)
{
    switch (hr)
    {
    //COMMON ERRORS
    case S_OK:
        return "Operation succesful";
        break;
    case S_FALSE:
        return "Alternate success value, indicating a successful but nonstandard completion (the precise meaning depends on context)";
        break;
    case E_FAIL:
        return "Unspecified failure";
        break;
    case E_INVALIDARG:
        return "One or more arguments are not valid";
        break;
    case E_NOTIMPL:
        return "Not implemented";
        break;
    case E_OUTOFMEMORY:
        return "Failed to allocate necessary memory";
        break;

    //D3D ERRORS
    case D3D11_ERROR_FILE_NOT_FOUND:
        return "The file was not found";
        break;
    case D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS:
        return "There are too many unique instances of a particular type of state object";
        break;
    case D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS:
        return     "There are too many unique instances of a particular type of view object";
        break;
    case D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD:
        return "The first call to ID3D11DeviceContext::Map after either ID3D11Device::CreateDeferredContext or ID3D11DeviceContext::FinishCommandList per Resource was not D3D11_MAP_WRITE_DISCARD";
        break;

    //DXGI ERRORS
    case DXGI_ERROR_INVALID_CALL:
        return "The method call is invalid. For example, a method's parameter may not be a valid pointer";
        break;
    case DXGI_ERROR_WAS_STILL_DRAWING:
        return "The previous blit operation that is transferring information to or from this surface is incomplete";
        break;
    case DXGI_ERROR_DEVICE_HUNG:
        return "The application's device failed due to badly formed commands sent by the application. This is an design-time issue that should be investigated and fixed";
        break;
    case DXGI_ERROR_DEVICE_REMOVED:
        return "The video card has been physically removed from the system, or a driver upgrade for the video card has occurred.The application should destroy and recreate the device.For help debugging the problem, call ID3D10Device::GetDeviceRemovedReason.";
        break;
    case DXGI_ERROR_DEVICE_RESET:
        return "The device failed due to a badly formed command. This is a run-time issue; The application should destroy and recreate the device";
        break;
    case DXGI_ERROR_DRIVER_INTERNAL_ERROR:
        return "The driver encountered a problem and was put into the device removed state";
        break;
    case DXGI_ERROR_FRAME_STATISTICS_DISJOINT:
        return "An event (for example, a power cycle) interrupted the gathering of presentation statistics";
        break;
    case DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE:
        return "The application attempted to acquire exclusive ownership of an output, but failed because some other application (or device within the application) already acquired ownership";
        break;
    case DXGI_ERROR_MORE_DATA:
        return "The buffer supplied by the application is not big enough to hold the requested data";
        break;
    case DXGI_ERROR_NONEXCLUSIVE:
        return "A global counter resource is in use, and the Direct3D device can't currently use the counter resource";
        break;
    case DXGI_ERROR_NOT_CURRENTLY_AVAILABLE:
        return "The resource or request is not currently available, but it might become available later";
        break;
    case DXGI_ERROR_NOT_FOUND:
        return "When calling IDXGIObject::GetPrivateData, the GUID passed in is not recognized as one previously passed to IDXGIObject::SetPrivateData or IDXGIObject::SetPrivateDataInterface. When calling IDXGIFactory::EnumAdapters or IDXGIAdapter::EnumOutputs, the enumerated ordinal is out of range.";
        break;
    case DXGI_ERROR_UNSUPPORTED:
        return "The requested functionality is not supported by the device or the driver";
        break;
    case DXGI_ERROR_ACCESS_LOST:
        return "The desktop duplication interface is invalid. The desktop duplication interface typically becomes invalid when a different type of image is displayed on the desktop";
        break;
    case DXGI_ERROR_WAIT_TIMEOUT:
        return "The time-out interval elapsed before the next desktop frame was available";
        break;
    case DXGI_ERROR_SESSION_DISCONNECTED:
        return "The Remote Desktop Services session is currently disconnected";
        break;
    case DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE:
        return "The DXGI output (monitor) to which the swap chain content was restricted is now disconnected or changed";
        break;
    case DXGI_ERROR_ACCESS_DENIED:
        return "You tried to use a resource to which you did not have the required access privileges. This error is most typically caused when you write to a shared resource with read-only access";
        break;
    case DXGI_ERROR_NAME_ALREADY_EXISTS:
        return "The supplied name of a resource in a call to IDXGIResource1::CreateSharedHandle is already associated with some other resource";
        break;
    case DXGI_ERROR_SDK_COMPONENT_MISSING:
        return "The operation depends on an SDK component that is missing or mismatched";
        break;

    //PRESENT STATUS
    case DXGI_STATUS_OCCLUDED:
        return "The window content is not visible. When receiving this status, an application can stop rendering and use DXGI_PRESENT_TEST to determine when to resume rendering";
        break;
    case DXGI_STATUS_MODE_CHANGED:
        return "The desktop display mode has been changed, there might be color conversion/stretching. The application should call IDXGISwapChain::ResizeBuffers to match the new display mode";
        break;
    case DXGI_STATUS_MODE_CHANGE_IN_PROGRESS:
        return "IDXGISwapChain::ResizeTarget and IDXGISwapChain::SetFullscreenState will return DXGI_STATUS_MODE_CHANGE_IN_PROGRESS if a fullscreen/windowed mode transition is occurring when either API is called";
        break;
    }

    return "Invalid hresult value " + std::to_string(hr);
}