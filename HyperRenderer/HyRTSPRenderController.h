#pragma once
#define WIN32_LEAN_AND_MEAN
#include <memory>

#include "HyD2DHandler.h"
#include "HyRTSPManager.h"

class HyRTSPRenderController{
public:
	HyRTSPRenderController(HWND hWnd);
	~HyRTSPRenderController();

	bool Initialize();
	bool OnRender();
	void OnResize(UINT width, UINT height);

private:
	std::unique_ptr<HyRTSPManager> rtsp;
	std::unique_ptr<HyD2DHandler> d2d;
};
