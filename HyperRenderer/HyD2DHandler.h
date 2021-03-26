#pragma once
#include <d2d1.h>
#include <d2d1helper.h>
#include <wincodec.h>
#include "HyFrame.h"

class HyD2DHandler
{
public:
	HyD2DHandler(HWND hWnd); // ctor
	~HyD2DHandler(void); // dtor
	HRESULT Initialize(); // Initialize the rendering
	HRESULT OnRender(HyFrame *frame); // Called from OnPaint function
	void OnResize(UINT width, UINT height);

private:
	HRESULT CreateDeviceResources(); // Create resources for drawing
	void DiscardDeviceResources(); // Release resources for drawing

private:

	HWND					m_hWnd;
	ID2D1Factory*			m_pDirect2dFactory;
	ID2D1HwndRenderTarget*	m_pRenderTarget;
	IWICImagingFactory*		m_pWicImgFactory;
	ID2D1SolidColorBrush*	m_pLightSlateGrayBrush;
	ID2D1LinearGradientBrush* m_pLinearGradientBrush;

	ID2D1Bitmap *m_pRenderBitmap;
};