#define WIN32_LEAN_AND_MEAN

#include <d2d1.h>
#include <d2d1helper.h>
#include <memory>

#include "HyRTSPRenderController.h"

#define HYPER_SCREEN_API extern "C" __declspec(dllexport)

using namespace std;

HMODULE gMainModule = nullptr;
std::unique_ptr<HyRTSPRenderController> controller = nullptr;

inline bool Guard() { return controller != nullptr; }

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
	controller = make_unique<HyRTSPRenderController>(hWnd);

	return controller != nullptr ? 0 : -1;
}

HYPER_SCREEN_API int __stdcall Initialize()
{
	if (!Guard()) return -1;

	return controller->Initialize() ? 0 : -1;
}

HYPER_SCREEN_API int __stdcall OnRender()
{
	if (!Guard()) return -1;

	return controller->OnRender() ? 0 : -1;
}

HYPER_SCREEN_API int __stdcall OnResize(UINT width, UINT height)
{
	if (!Guard()) return -1;

	controller->OnResize(width, height);
	printf("width: %u / height: %u\n", width, height);

	return 0;
}
