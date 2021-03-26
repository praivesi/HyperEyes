#pragma once
#define WIN32_LEAN_AND_MEAN
#include <memory>
#include <functional>

#include "HyD2DHandler.h"
#include "HyRTSPManager.h"
#include "CommonDefines.h"
#include "HyFrame.h"

class HyRTSPRenderController{
public:
	HyRTSPRenderController(HWND hWnd);
	~HyRTSPRenderController();

	bool Initialize();
	bool OnRender();
	void OnResize(UINT width, UINT height);

	void Play();

	void recvFrameImp(HyFrame *frame);
	//static void recvFrameImp(HyFrame *frame);

	//void recvFrameImp(uint8_t* pReceiveBuffer, char* pStreamId, unsigned frameSize, unsigned numTruncatedBytes,
	//	struct timeval presentationTime, unsigned durationInMicroseconds);

private:
	/*static HyRTSPManager* rtsp;
	static HyD2DHandler* d2d;*/
	std::unique_ptr<HyRTSPManager> rtsp;
	std::unique_ptr<HyD2DHandler> d2d;
};
