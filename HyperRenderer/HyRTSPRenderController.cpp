#include "HyRTSPRenderController.h"

//void recvFrameImp(HyFrame *frame);

HyRTSPRenderController::HyRTSPRenderController(HWND hWnd)
{
	rtsp = std::make_unique<HyRTSPManager>();
	d2d = std::make_unique<HyD2DHandler>(hWnd);
	//rtsp = new HyRTSPManager();
	//d2d = new HyD2DHandler(hWnd);
}

HyRTSPRenderController::~HyRTSPRenderController() {}

void HyRTSPRenderController::Play()
{
#pragma region DO RTSP
	HyRTSPManager rtspManager;

	std::string rtspURL = "rtsp://admin:1234@192.168.21.203:554/video1";

	//rtspManager.beginStreaming(rtspURL, &recvFrameImp);
	//recvHandler rh = &this->recvFrameImp;
	recvHandler rh = std::bind(&this->recvFrameImp);
	rtspManager.beginStreaming(rtspURL, rh);
#pragma endregion
}

bool HyRTSPRenderController::Initialize()
{
	return SUCCEEDED(d2d->Initialize());
}

bool HyRTSPRenderController::OnRender()
{
	return true;
	//return SUCCEEDED(d2d->OnRender());
}

void HyRTSPRenderController::OnResize(UINT width, UINT height)
{
	d2d->OnResize(width, height);
}

void HyRTSPRenderController::recvFrameImp(HyFrame *frame)
{
	//printf("%d\n", frame->frame_id);
	d2d->OnRender(frame);
	//DummySink* sink = (DummySink*)clientData;

	//printf("fStreamId => %s\n", pStreamId);
	//printf("pReceivedBuffer => %s\n", pReceiveBuffer);

	//printf("RECIEVED FRAME BY ME : %u, %u\n", frameSize, numTruncatedBytes);
}
