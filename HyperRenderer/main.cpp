#include <iostream>
#include <vector>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include "HyRTSPManager.h"

int main()
{
	//AVCodec* av_codec;
	HyRTSPManager rtspManager;

	std::string rtspURL = "rtsp://admin:1234@192.168.21.203:554/video1";
	
	rtspManager.beginStreaming(rtspURL, &recvFrameImp);

	return 0;
}

void recvFrameImp(unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime)
{
	printf("RECIEVED FRAME BY ME : %u, %u\n", frameSize, numTruncatedBytes);
}
