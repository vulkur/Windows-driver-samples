#include <errno.h>
#include <stdlib.h>

#include <iostream>
#include <vector>

#include <windows.h>
#include <swdevice.h>
#include <conio.h>
#include <wrl.h>

#include <stdbool.h>

#pragma warning(disable : 4201)
#pragma warning(disable : 4324)

#include <d3d11_4.h>
#include <d3dcommon.h>
#include <dxgi1_6.h>

#pragma warning(default : 4201)
#pragma warning(default : 4324)

void test_d3d11()
{
    HRESULT hr = S_OK;

    ID3D11Device* device = NULL;
    ID3D11DeviceContext* device_ctx = NULL;

    hr = D3D11CreateDevice(NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        NULL,
        0,
        D3D11_SDK_VERSION,
        &device,
        NULL,
        &device_ctx);

    if (FAILED(hr))
    {
        char* error_mssg = NULL;

        if(FACILITY_WINDOWS == HRESULT_FACILITY(hr))
        {
            hr = HRESULT_CODE(hr);
        }

        if (FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR) &error_mssg, 256, nullptr) != 0)
        {
            printf("D3D11 Device create FAILED: %x; %s\n", hr, error_mssg);
            LocalFree(error_mssg);
        }
        else
        {
            printf("D3D11 Device create FAILED : %x\n", hr);
        }
    }
    else
    {
        printf("D3D11 Device created\n");
    }
}

VOID WINAPI
CreationCallback(
    _In_ HSWDEVICE hSwDevice,
    _In_ HRESULT hrCreateResult,
    _In_opt_ PVOID pContext,
    _In_opt_ PCWSTR pszDeviceInstanceId
    )
{
    if (pContext)
    {
        HANDLE hEvent = *((HANDLE*) pContext);

        SetEvent(hEvent);
    }

    UNREFERENCED_PARAMETER(hSwDevice);
    UNREFERENCED_PARAMETER(hrCreateResult);
    UNREFERENCED_PARAMETER(pszDeviceInstanceId);
}

int __cdecl main(int argc, char *argv[])
{
    DWORD loop_count = 50;


    HANDLE hEvent;
    HSWDEVICE hSwDevice;
    SW_DEVICE_CREATE_INFO createInfo = { 0 };
    PCWSTR description = L"Idd Sample Driver";

    // These match the Pnp id's in the inf file so OS will load the driver when the device is created    
    PCWSTR instanceId = L"IddSampleDriver";
    PCWSTR hardwareIds = L"IddSampleDriver\0\0";
    PCWSTR compatibleIds = L"IddSampleDriver\0\0";

    if (argc > 1)
    {
        errno = 0;
        DWORD temp = strtol(argv[1], nullptr, 10);
        if (errno == 0)
        {
            loop_count = temp;
        }
    }

    // Lets run one test before we even create a SW device
    test_d3d11();

    createInfo.cbSize = sizeof(createInfo);
    createInfo.pszzCompatibleIds = compatibleIds;
    createInfo.pszInstanceId = instanceId;
    createInfo.pszzHardwareIds = hardwareIds;
    createInfo.pszDeviceDescription = description;

    createInfo.CapabilityFlags = SWDeviceCapabilitiesRemovable |
                                 SWDeviceCapabilitiesSilentInstall |
                                 SWDeviceCapabilitiesDriverRequired;

    hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!hEvent)
    {
        printf("Failed to create event");
        return -1;
    }

    printf("Looping %u times\n", loop_count);
    for(DWORD i = 0; i < loop_count; i++)
    {
        // Create the device
        HRESULT hr = SwDeviceCreate(L"IddSampleDriver",
                                    L"HTREE\\ROOT\\0",
                                    &createInfo,
                                    0,
                                    nullptr,
                                    CreationCallback,
                                    &hEvent,
                                    &hSwDevice);
        if (FAILED(hr))
        {
            printf("SwDeviceCreate failed with 0x%lx\n", hr);
            return 1;
        }

        DWORD waitResult = WaitForSingleObject(hEvent, 10*1000);
        if (waitResult != WAIT_OBJECT_0)
        {
            printf("Wait for device creation failed\n");
            return 1;
        }

        printf("SW Device created: %u\n", i);

        Sleep(2500);

        test_d3d11();

        SwDeviceClose(hSwDevice);
    }

    CloseHandle(hEvent);

    return 0;
}
