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

private:
	HyRTSPManager* rtsp;
	HyD2DHandler* d2d;
};
