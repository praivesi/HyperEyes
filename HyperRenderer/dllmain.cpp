#include <d2d1.h>
#include <d2d1helper.h>
#include <memory>

#include "HyD2DHandler.h"


#define HYPER_SCREEN_API extern "C" __declspec(dllexport)

using namespace std;

HMODULE gMainModule = nullptr;
unique_ptr<HyD2DHandler> d2d = nullptr;

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ulReasonForCall,
	LPVOID lpReserved
) {
	switch (ulReasonForCall) {
	case DLL_PROCESS_ATTACH:
		gMainModule = hModule;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

HYPER_SCREEN_API int __stdcall CreateInstance(HWND hWnd)
{
	d2d = make_unique<HyD2DHandler>(hWnd);

	return d2d != nullptr ? 0 : -1;
}

HYPER_SCREEN_API int __stdcall Initialize()
{
	HRESULT hr = d2d->Initialize();

	return SUCCEEDED(hr) ? 0 : -1;
}

HYPER_SCREEN_API int __stdcall OnRender()
{
	HRESULT hr = d2d->OnRender();

	return SUCCEEDED(hr) ? 0 : -1;
}

HYPER_SCREEN_API int __stdcall OnResize(UINT width, UINT height)
{
	d2d->OnResize(width, height);
	printf("width: %u / height: %u\n", width, height);

	return 0;
}
