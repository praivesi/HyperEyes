#pragma once

#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"

#include "HyRTSPClient.h"
#include "CommonDefines.h"

#include <iostream>
#include <vector>

void recvFrameImp(unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime);

class HyRTSPManager {
public:
	bool beginStreaming(std::vector<std::string> rtspURLs);

private:
	// The main streaming routine (for each "rtsp://" URL):
	void openURL(UsageEnvironment& env, char const* progName, char const* rtspURL);

	void (*recvFrame)(unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime);

	recvHandler handler;
};
