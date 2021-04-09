#pragma once
#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"

#include "HyRTSPClient.h"
#include "CommonDefines.h"

#include <iostream>
#include <vector>
#include <functional>

class HyRTSPManager {
public:
	HyRTSPManager();
	~HyRTSPManager();
	bool beginStreaming(std::string rtspURL, std::function<void(HyFrame*)> funcPtr);

private:
	// The main streaming routine (for each "rtsp://" URL):
	void openURL(UsageEnvironment& env, char const* progName, char const* rtspURL, std::function<void(HyFrame*)> recvCallback);

	void (*recvFrame)(unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime);
};
