#include "HyRTSPManager.h"

char eventLoopWatchVariable = 0;

HyRTSPManager::HyRTSPManager()
{

}

HyRTSPManager::~HyRTSPManager()
{

}

bool HyRTSPManager::beginStreaming(std::string rtspURL, recvHandler recvCallback)
{
	// Begin by setting up our usage environment:
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);

	//if (rtspURLs.size() == 0)
	//{
	//	*env << "There's no RTSP URL." << "\n";
	//	return false;
	//}

	std::string progName = "HyRTSPClient";
	//for (std::string & rtspURL : rtspURLs)
	//{
	//	openURL(*env, progName.c_str(), rtspURL.c_str(), recvCallback);
	//}
	openURL(*env, progName.c_str(), rtspURL.c_str(), recvCallback);

	// All subsequent activity takes place within the event loop:
	env->taskScheduler().doEventLoop(&eventLoopWatchVariable);
	// This function call does not return, unless, at some point in time, "eventLoopWatchVariable" gets set to something non-zero.

	return true;

	// If you choose to continue the application past this point (i.e., if you comment out the "return 0;" statement above),
	// and if you don't intend to do anything more with the "TaskScheduler" and "UsageEnvironment" objects,
	// then you can also reclaim the (small) memory used by these objects by uncommenting the following code:
	/*
	env->reclaim(); env = NULL;
	delete scheduler; scheduler = NULL;
	*/
}

static unsigned rtspClientCount = 0; // Counts how many streams (i.e., "RTSPClient"s) are currently in use.

void HyRTSPManager::openURL(UsageEnvironment& env, char const* progName, char const* rtspURL, recvHandler recvCallback) {
	// Begin by creating a "RTSPClient" object.  Note that there is a separate "RTSPClient" object for each stream that we wish
	// to receive (even if more than stream uses the same "rtsp://" URL).

	//RTSPClient* rtspClient = HyRTSPClient::createNew(env, rtspURL, &recvFrameImp, RTSP_CLIENT_VERBOSITY_LEVEL, progName);
	RTSPClient* rtspClient = HyRTSPClient::createNew(env, rtspURL, recvCallback, RTSP_CLIENT_VERBOSITY_LEVEL, progName);
	if (rtspClient == NULL) {
		env << "Failed to create a RTSP client for URL \"" << rtspURL << "\": " << env.getResultMsg() << "\n";
		return;
	}

	++rtspClientCount;

	// Next, send a RTSP "DESCRIBE" command, to get a SDP description for the stream.
	// Note that this command - like all RTSP commands - is sent asynchronously; we do not block, waiting for a response.
	// Instead, the following function call returns immediately, and we handle the RTSP response later, from within the event loop:
	rtspClient->sendDescribeCommand(continueAfterDESCRIBE);
}

//void recvFrameImp(unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime)
//{
//	printf("RECIEVED FRAME BY ME : %u, %u\n", frameSize, numTruncatedBytes);
//}