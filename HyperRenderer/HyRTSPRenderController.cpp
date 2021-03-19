#include "HyRTSPRenderController.h"

HyRTSPRenderController::HyRTSPRenderController(HWND hWnd)
{
	rtsp = std::make_unique<HyRTSPManager>();
	d2d = std::make_unique<HyD2DHandler>(hWnd);
}

HyRTSPRenderController::~HyRTSPRenderController() {}

bool HyRTSPRenderController::Initialize()
{
	return SUCCEEDED(d2d->Initialize());
}

bool HyRTSPRenderController::OnRender()
{
	return SUCCEEDED(d2d->OnRender());
}

void HyRTSPRenderController::OnResize(UINT width, UINT height)
{
	d2d->OnResize(width, height);
}
