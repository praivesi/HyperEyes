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

	std::vector<std::string> rtspURLs;
	rtspURLs.push_back("rtsp://admin:1234@192.168.21.203:554/video1");
	
	rtspManager.BeginStreaming(rtspURLs);

	return 0;
}
