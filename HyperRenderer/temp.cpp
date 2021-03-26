void CameraStream::Play(CWnd *view)
{
	m_View = view;
	m_StreamThread = std::thread{
		[&, this]()
	{
		AVPacket packet;
		int i = 0;
		while (true)
		{
			{
				std::unique_lock<std::mutex> threadMux(this->m_StreamMux);
				if (!m_IsStreamAlive)
				{
					MessageBox(nullptr, _T("ThreadOut"), nullptr, MB_OK);
					break;
				}
			}
			int err = av_read_frame(this->m_FormatContext, &packet);
			if (err < 0)
			{
				break;
			}
			if (packet.stream_index == m_VideoStreamIdx)
			{
				err = avcodec_send_packet(this->m_VideoCondecContext, &packet);
				if (err < 0
					|| err == AVERROR(EAGAIN)
					|| err == AVERROR_EOF)
				{
					break;
				}
				CDC *dc = ((CNetworkVideoViewerView*)m_View)->GetDC();
				BYTE *pbmpdata = nullptr;
				BITMAPINFO bmi = { 0 };
				bmi.bmiHeader.biBitCount = 24;
				bmi.bmiHeader.biCompression = BI_RGB;
				bmi.bmiHeader.biHeight = -(m_VideoCondecContext->height);
				bmi.bmiHeader.biWidth = m_VideoCondecContext->width;
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bmi.bmiHeader.biSizeImage = m_VideoCondecContext->height * m_VideoCondecContext->width * 3;
				HBITMAP hbmp = CreateDIBSection(dc->GetSafeHdc(), &bmi, DIB_RGB_COLORS, (void**)&pbmpdata, NULL, 0);
				AVFrame *frame = av_frame_alloc();;
				AVFrame *frameRGB = av_frame_alloc();
				uint8_t *buffer = nullptr;
				int numBytes = 0;
				numBytes = av_image_get_buffer_size(
					AV_PIX_FMT_BGR24,
					this->m_VideoCondecContext->width,
					this->m_VideoCondecContext->height,
					1);
				buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
				int err = av_image_fill_arrays(
					frameRGB->data,
					frameRGB->linesize,
					(uint8_t*)pbmpdata, // Bitmap Pointer
										//buffer,
					AV_PIX_FMT_BGR24,
					this->m_VideoCondecContext->width,
					this->m_VideoCondecContext->height,
					1);
				if (err < 0)
				{
					break;
				}
				SwsContext *swsContext = nullptr;
				swsContext = sws_getContext(
					this->m_VideoCondecContext->width,
					this->m_VideoCondecContext->height,
					this->m_VideoCondecContext->pix_fmt,
					this->m_VideoCondecContext->width,
					this->m_VideoCondecContext->height,
					AV_PIX_FMT_BGR24,
					SWS_BICUBIC,
					nullptr,
					nullptr,
					nullptr);
				if (!swsContext)
				{
					return false;
				}
				while (err >= 0)
				{
					err = avcodec_receive_frame(this->m_VideoCondecContext, frame);
					if (err == AVERROR(EAGAIN)
						|| err == AVERROR(EINVAL)
						|| err == AVERROR_EOF)
					{
						break;
					}
					if (frame->pkt_size != 0)
					{
						sws_scale(
							swsContext,
							(const uint8_t * const *)frame->data,
							frame->linesize,
							0,
							this->m_VideoCondecContext->height,
							frameRGB->data,
							frameRGB->linesize);
						// SendMessage Main Thread.
						((CNetworkVideoViewerView*)(m_View))->SendMessage(
							UM_DISPLAYBITMAP,
							(WPARAM)hbmp);
					}
				}
				av_frame_free(&frame);
				av_frame_free(&frameRGB);
				sws_freeContext(swsContext);
				av_free((void*)buffer);
				((CNetworkVideoViewerView*)m_View)->ReleaseDC(dc);
			}
			av_packet_unref(&packet);
		};
	} };
};
NetWorkViewerView.cpp
// UM_DISPLAYBITMAP
afx_msg LRESULT CNetworkVideoViewerView::OnUmDisplaybitmap(WPARAM wParam, LPARAM lParam)
{
	HBITMAP *hBmp = (HBITMAP*)wParam;
	CDC *dc = GetDC();
	CDC memDC;
	memDC.CreateCompatibleDC(dc);
	BITMAP bmpInfo;
	GetObject(hBmp, sizeof(BITMAP), &bmpInfo);
	memDC.SelectObject(hBmp);
	dc->BitBlt(0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, &memDC, 0, 0, SRCCOPY);
	DeleteObject(hBmp);
	ReleaseDC(dc);
	return 0;
}