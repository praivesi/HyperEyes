#include "HyRTSPRenderController.h"

HyRTSPRenderController::HyRTSPRenderController(HWND hWnd)
{
	rtsp = new HyRTSPManager();
	d2d = new HyD2DHandler(hWnd);
}

HyRTSPRenderController::~HyRTSPRenderController() {}

void HyRTSPRenderController::Play()
{
#pragma region DO RTSP
	HyRTSPManager rtspManager;

	std::string rtspURL = "rtsp://admin:1234@192.168.21.203:554/video1";

	rtspManager.beginStreaming(rtspURL, std::bind(&HyRTSPRenderController::recvFrameImp, this, std::placeholders::_1));
#pragma endregion
}

bool HyRTSPRenderController::Initialize()
{
	return SUCCEEDED(d2d->Initialize());
}

bool HyRTSPRenderController::OnRender()
{
	return true;
}

void HyRTSPRenderController::OnResize(UINT width, UINT height)
{
	d2d->OnResize(width, height);
}

void HyRTSPRenderController::recvFrameImp(HyFrame *frame)
{
	d2d->OnRender(frame);
}
