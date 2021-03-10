#pragma once

#include <d2d1.h>
#include <d2d1_1.h>
#include <wincodec.h>
#include <memory>
#include <combaseapi.h>
#include <thread>
#include <mutex>
#include <iostream>

#include "HPTimer.h"
#include "CommonFunctions.h"
#include "HyFrame.h"

const float DEFAULT_DPI = 96.f;   // Default DPI that maps image resolution directly to screen resoltuion
const int RENDERING_GAP = 14;

class HyRenderer {
private:
	ID2D1Factory* m_d2d_factory;
	IWICImagingFactory* m_iwic_factory;
	ID2D1HwndRenderTarget* m_render_target_handle;
	ID2D1Multithread* m_d2d_multithread; 	// thread lock
	ID2D1SolidColorBrush* m_channel_background_brush;

	ID2D1Bitmap* m_image_bitmap;
	D2D1_SIZE_F m_render_target_size;

	HWND m_client_handle;

	HPTimer* hp_timer;
	std::thread m_worker_thread;
	std::mutex m_worker_thread_mutex;

	bool m_is_started;
	bool m_is_keep_aspect_ratio;
	bool m_is_flip;
	bool m_is_stop_requested;

public:
	HyRenderer();
	~HyRenderer();

private:
	HRESULT CreateDeviceResources();
	HRESULT CreateBrushes();

	void StartRendering();
	void StopRendering();
	HRESULT DoRendering(); // Main Rendering Function

	void SetBitmap(const HyFrame* frame);
	void PaintCallback();
};
