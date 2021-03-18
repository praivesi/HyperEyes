#include <iostream>
#include <vector>

//extern "C"
//{
//	#include <libavcodec/avcodec.h>
//	#include <libavformat/avformat.h>
//	#include <libswscale/swscale.h>
//}

#include "HyRTSPManager.h"

void recvFrameImp(uint8_t* pReceiveBuffer, char* pStreamId, unsigned frameSize, unsigned numTruncatedBytes,
	struct timeval presentationTime, unsigned durationInMicroseconds);

int main()
{
	AVFormatContext *pFormatCtx = nullptr;

	//if (avformat_open_input(&pFormatCtx, "test.h264", nullptr, 0) != 0) { return -1; }
	//if (avformat_find_stream_info(pFormatCtx, nullptr) < 0) { return -1; }

	//AVCodecContext* pCodecCtxOrig = nullptr;
	//AVCodecContext* pCodecCtx = nullptr;

	//// Find the first video stream
	//int videoStream = -1;
	//for (int i = 0; i < pFormatCtx->nb_streams; i++)
	//{
	//	if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
	//	{
	//		videoStream = i;
	//		break;
	//	}
	//}

	//if (videoStream == -1) { return -1; }

	//pCodecCtx = pFormatCtx->streams[videoStream]->codec;

	//AVCodec* pCodec = nullptr;
	//
	//pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	//if (pCodec == nullptr)
	//{
	//	fprintf(stderr, "Unsupported codec!\n");
	//	return -1;
	//}

	//pCodecCtx = avcodec_alloc_context3(pCodec);
	//if (avcodec_copy_context(pCodecCtx, pCodecCtxOrig) != 0)
	//{
	//	fprintf(stderr, "Couldn't copy codec context");
	//	return -1;
	//}

	//if (avcodec_open2(pCodecCtx, pCodec, nullptr) < 0) { return -1; }

	//AVFrame* pFrame = nullptr;
	//AVFrame* pFrameRGB = nullptr;

	//pFrameRGB = av_frame_alloc();
	//if (pFrameRGB == nullptr) { return -1; }

	//uint8_t* buffer = nullptr;
	//int numBytes;

	//numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
	//buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));

	//avpicture_fill((AVPicture*)pFrameRGB, buffer, AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);

	//struct SwsContext* sws_ctx = nullptr;
	//int frameFinished;
	//AVPacket packet;

	//sws_ctx = sws_getContext(pCodecCtx->width,
	//	pCodecCtx->height,
	//	pCodecCtx->pix_fmt,
	//	pCodecCtx->width,
	//	pCodecCtx->height,
	//	AV_PIX_FMT_RGB24,
	//	SWS_BILINEAR,
	//	NULL,
	//	NULL,
	//	NULL);

	//int i = 0;
	//while (av_read_frame(pFormatCtx, &packet) >= 0)
	//{
	//	if (packet.stream_index == videoStream)
	//	{
	//		avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

	//		if (frameFinished)
	//		{
	//			sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data,
	//				pFrame->linesize, 0, pCodecCtx->height,
	//				pFrameRGB->data, pFrameRGB->linesize);

	//			if (++i <= 5)
	//			{
	//				// Save
	//			}
	//		}
	//	}

	//	av_free_packet(&packet);
	//}

	//av_free(buffer);
	//av_free(pFrameRGB);
	//av_free(pFrame);

	//avcodec_close(pCodecCtx);
	//avcodec_close(pCodecCtxOrig);

	//avformat_close_input(&pFormatCtx);

#pragma region DO RTSP
	HyRTSPManager rtspManager;

	std::string rtspURL = "rtsp://admin:1234@192.168.21.203:554/video1";
	
	rtspManager.beginStreaming(rtspURL, &recvFrameImp);
#pragma endregion

	return 0;
}

void recvFrameImp(uint8_t* pReceiveBuffer, char* pStreamId, unsigned frameSize, unsigned numTruncatedBytes,
	struct timeval presentationTime, unsigned durationInMicroseconds)
{
	//DummySink* sink = (DummySink*)clientData;

	//printf("fStreamId => %s\n", pStreamId);
	//printf("pReceivedBuffer => %s\n", pReceiveBuffer);

	//printf("RECIEVED FRAME BY ME : %u, %u\n", frameSize, numTruncatedBytes);
}
