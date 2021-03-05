#pragma once

#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"

#include "HyRTSPClient.h"

#include <iostream>
#include <vector>

class HyRTSPManager {
public:
	bool BeginStreaming(std::vector<std::string> rtspURLs);

private:
	// The main streaming routine (for each "rtsp://" URL):
	void openURL(UsageEnvironment& env, char const* progName, char const* rtspURL);
};
