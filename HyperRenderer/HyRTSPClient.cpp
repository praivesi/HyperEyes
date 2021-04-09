#include "HyRTSPClient.h"

// A function that outputs a string that identifies each stream (for debugging output).  Modify this if you wish:
UsageEnvironment& operator<<(UsageEnvironment& env, const RTSPClient& rtspClient) {
	return env << "[URL:\"" << rtspClient.url() << "\"]: ";
}

// A function that outputs a string that identifies each subsession (for debugging output).  Modify this if you wish:
UsageEnvironment& operator<<(UsageEnvironment& env, const MediaSubsession& subsession) {
	return env << subsession.mediumName() << "/" << subsession.codecName();
}

void usage(UsageEnvironment& env, char const* progName) {
	env << "Usage: " << progName << " <rtsp-url-1> ... <rtsp-url-N>\n";
	env << "\t(where each <rtsp-url-i> is a \"rtsp://\" URL)\n";
}

// Implementation of the RTSP 'response handlers':

void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString) {
	do {
		UsageEnvironment& env = rtspClient->envir(); // alias
		StreamClientState& scs = ((HyRTSPClient*)rtspClient)->scs; // alias

		if (resultCode != 0) {
			env << *rtspClient << "Failed to get a SDP description: " << resultString << "\n";
			delete[] resultString;
			break;
		}

		char* const sdpDescription = resultString;
		env << *rtspClient << "Got a SDP description:\n" << sdpDescription << "\n";

		// Create a media session object from this SDP description:
		scs.session = MediaSession::createNew(env, sdpDescription);
		delete[] sdpDescription; // because we don't need it anymore
		if (scs.session == NULL) {
			env << *rtspClient << "Failed to create a MediaSession object from the SDP description: " << env.getResultMsg() << "\n";
			break;
		}
		else if (!scs.session->hasSubsessions()) {
			env << *rtspClient << "This session has no media subsessions (i.e., no \"m=\" lines)\n";
			break;
		}

		// Then, create and set up our data source objects for the session.  We do this by iterating over the session's 'subsessions',
		// calling "MediaSubsession::initiate()", and then sending a RTSP "SETUP" command, on each one.
		// (Each 'subsession' will have its own data source.)
		scs.iter = new MediaSubsessionIterator(*scs.session);
		setupNextSubsession(rtspClient);
		return;
	} while (0);

	// An unrecoverable error occurred with this stream.
	shutdownStream(rtspClient);
}

// By default, we request that the server stream its data using RTP/UDP.
// If, instead, you want to request that the server stream via RTP-over-TCP, change the following to True:
#define REQUEST_STREAMING_OVER_TCP False

void setupNextSubsession(RTSPClient* rtspClient) {
	UsageEnvironment& env = rtspClient->envir(); // alias
	StreamClientState& scs = ((HyRTSPClient*)rtspClient)->scs; // alias

	scs.subsession = scs.iter->next();
	if (scs.subsession != NULL) {
		if (!scs.subsession->initiate()) {
			env << *rtspClient << "Failed to initiate the \"" << *scs.subsession << "\" subsession: " << env.getResultMsg() << "\n";
			setupNextSubsession(rtspClient); // give up on this subsession; go to the next one
		}
		else {
			env << *rtspClient << "Initiated the \"" << *scs.subsession << "\" subsession (";
			if (scs.subsession->rtcpIsMuxed()) {
				env << "client port " << scs.subsession->clientPortNum();
			}
			else {
				env << "client ports " << scs.subsession->clientPortNum() << "-" << scs.subsession->clientPortNum() + 1;
			}
			env << ")\n";

			// Continue setting up this subsession, by sending a RTSP "SETUP" command:
			rtspClient->sendSetupCommand(*scs.subsession, continueAfterSETUP, False, REQUEST_STREAMING_OVER_TCP);
		}
		return;
	}

	// We've finished setting up all of the subsessions.  Now, send a RTSP "PLAY" command to start the streaming:
	if (scs.session->absStartTime() != NULL) {
		// Special case: The stream is indexed by 'absolute' time, so send an appropriate "PLAY" command:
		rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY, scs.session->absStartTime(), scs.session->absEndTime());
	}
	else {
		scs.duration = scs.session->playEndTime() - scs.session->playStartTime();
		rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY);
	}
}

void continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString) {
	do {
		UsageEnvironment& env = rtspClient->envir(); // alias
		StreamClientState& scs = ((HyRTSPClient*)rtspClient)->scs; // alias

		if (resultCode != 0) {
			env << *rtspClient << "Failed to set up the \"" << *scs.subsession << "\" subsession: " << resultString << "\n";
			break;
		}

		env << *rtspClient << "Set up the \"" << *scs.subsession << "\" subsession (";
		if (scs.subsession->rtcpIsMuxed()) {
			env << "client port " << scs.subsession->clientPortNum();
		}
		else {
			env << "client ports " << scs.subsession->clientPortNum() << "-" << scs.subsession->clientPortNum() + 1;
		}
		env << ")\n";

		// Having successfully setup the subsession, create a data sink for it, and call "startPlaying()" on it.
		// (This will prepare the data sink to receive data; the actual flow of data from the client won't start happening until later,
		// after we've sent a RTSP "PLAY" command.)

		scs.subsession->sink = DummySink::createNew(env, sendRecvFrameOnClient,
			*scs.subsession, rtspClient->url());
		// perhaps use your own custom "MediaSink" subclass instead
		if (scs.subsession->sink == NULL) {
			env << *rtspClient << "Failed to create a data sink for the \"" << *scs.subsession
				<< "\" subsession: " << env.getResultMsg() << "\n";
			break;
		}

		env << *rtspClient << "Created a data sink for the \"" << *scs.subsession << "\" subsession\n";
		scs.subsession->miscPtr = rtspClient; // a hack to let subsession handler functions get the "RTSPClient" from the subsession 
		scs.subsession->sink->startPlaying(*(scs.subsession->readSource()),
			subsessionAfterPlaying, scs.subsession);
		// Also set a handler to be called if a RTCP "BYE" arrives for this subsession:
		if (scs.subsession->rtcpInstance() != NULL) {
			scs.subsession->rtcpInstance()->setByeWithReasonHandler(subsessionByeHandler, scs.subsession);
		}
	} while (0);
	delete[] resultString;

	// Set up the next subsession, if any:
	setupNextSubsession(rtspClient);
}

void continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString) {
	Boolean success = False;

	do {
		UsageEnvironment& env = rtspClient->envir(); // alias
		StreamClientState& scs = ((HyRTSPClient*)rtspClient)->scs; // alias

		if (resultCode != 0) {
			env << *rtspClient << "Failed to start playing session: " << resultString << "\n";
			break;
		}

		// Set a timer to be handled at the end of the stream's expected duration (if the stream does not already signal its end
		// using a RTCP "BYE").  This is optional.  If, instead, you want to keep the stream active - e.g., so you can later
		// 'seek' back within it and do another RTSP "PLAY" - then you can omit this code.
		// (Alternatively, if you don't want to receive the entire stream, you could set this timer for some shorter value.)
		if (scs.duration > 0) {
			unsigned const delaySlop = 2; // number of seconds extra to delay, after the stream's expected duration.  (This is optional.)
			scs.duration += delaySlop;
			unsigned uSecsToDelay = (unsigned)(scs.duration * 1000000);
			scs.streamTimerTask = env.taskScheduler().scheduleDelayedTask(uSecsToDelay, (TaskFunc*)streamTimerHandler, rtspClient);
		}

		env << *rtspClient << "Started playing session";
		if (scs.duration > 0) {
			env << " (for up to " << scs.duration << " seconds)";
		}
		env << "...\n";

		success = True;
	} while (0);
	delete[] resultString;

	if (!success) {
		// An unrecoverable error occurred with this stream.
		shutdownStream(rtspClient);
	}
}


// Implementation of the other event handlers:

void subsessionAfterPlaying(void* clientData) {
	MediaSubsession* subsession = (MediaSubsession*)clientData;
	RTSPClient* rtspClient = (RTSPClient*)(subsession->miscPtr);

	// Begin by closing this subsession's stream:
	Medium::close(subsession->sink);
	subsession->sink = NULL;

	// Next, check whether *all* subsessions' streams have now been closed:
	MediaSession& session = subsession->parentSession();
	MediaSubsessionIterator iter(session);
	while ((subsession = iter.next()) != NULL) {
		if (subsession->sink != NULL) return; // this subsession is still active
	}

	// All subsessions' streams have now been closed, so shutdown the client:
	shutdownStream(rtspClient);
}

void subsessionByeHandler(void* clientData, char const* reason) {
	MediaSubsession* subsession = (MediaSubsession*)clientData;
	RTSPClient* rtspClient = (RTSPClient*)subsession->miscPtr;
	UsageEnvironment& env = rtspClient->envir(); // alias

	env << *rtspClient << "Received RTCP \"BYE\"";
	if (reason != NULL) {
		env << " (reason:\"" << reason << "\")";
		delete[] reason;
	}
	env << " on \"" << *subsession << "\" subsession\n";

	// Now act as if the subsession had closed:
	subsessionAfterPlaying(subsession);
}

void streamTimerHandler(void* clientData) {
	HyRTSPClient* rtspClient = (HyRTSPClient*)clientData;
	StreamClientState& scs = rtspClient->scs; // alias

	scs.streamTimerTask = NULL;

	// Shut down the stream:
	shutdownStream(rtspClient);
}

void shutdownStream(RTSPClient* rtspClient, int exitCode) {
	UsageEnvironment& env = rtspClient->envir(); // alias
	StreamClientState& scs = ((HyRTSPClient*)rtspClient)->scs; // alias

																// First, check whether any subsessions have still to be closed:
	if (scs.session != NULL) {
		Boolean someSubsessionsWereActive = False;
		MediaSubsessionIterator iter(*scs.session);
		MediaSubsession* subsession;

		while ((subsession = iter.next()) != NULL) {
			if (subsession->sink != NULL) {
				Medium::close(subsession->sink);
				subsession->sink = NULL;

				if (subsession->rtcpInstance() != NULL) {
					subsession->rtcpInstance()->setByeHandler(NULL, NULL); // in case the server sends a RTCP "BYE" while handling "TEARDOWN"
				}

				someSubsessionsWereActive = True;
			}
		}

		if (someSubsessionsWereActive) {
			// Send a RTSP "TEARDOWN" command, to tell the server to shutdown the stream.
			// Don't bother handling the response to the "TEARDOWN".
			rtspClient->sendTeardownCommand(*scs.session, NULL);
		}
	}

	env << *rtspClient << "Closing the stream.\n";
	Medium::close(rtspClient);
	// Note that this will also cause this stream's "StreamClientState" structure to get reclaimed.

	// [210305] TODO: Add managing RTSP client count
	//if (--rtspClientCount == 0) {
	//	// The final stream has ended, so exit the application now.
	//	// (Of course, if you're embedding this code into your own application, you might want to comment this out,
	//	// and replace it with "eventLoopWatchVariable = 1;", so that we leave the LIVE555 event loop, and continue running "main()".)
	//	exit(exitCode);
	//}
}


// Implementation of "HyRTSPClient":

HyRTSPClient* HyRTSPClient::createNew(UsageEnvironment& env, char const* rtspURL, std::function<void(HyFrame*)> recvCallback,
	int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum) {
	sendRecvFrameOnClient = recvCallback;


	return new HyRTSPClient(env, rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum);
}

HyRTSPClient::HyRTSPClient(UsageEnvironment& env, char const* rtspURL,
	int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum)
	: RTSPClient(env, rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum, -1) {
}

HyRTSPClient::~HyRTSPClient() {
}


// Implementation of "StreamClientState":

StreamClientState::StreamClientState()
	: iter(NULL), session(NULL), subsession(NULL), streamTimerTask(NULL), duration(0.0) {
}

StreamClientState::~StreamClientState() {
	delete iter;
	if (session != NULL) {
		// We also need to delete "session", and unschedule "streamTimerTask" (if set)
		UsageEnvironment& env = session->envir(); // alias

		env.taskScheduler().unscheduleDelayedTask(streamTimerTask);
		Medium::close(session);
	}
}

// Implementation of "DummySink":

// Even though we're not going to be doing anything with the incoming data, we still need to receive it.
// Define the size of the buffer that we'll use:
#define DUMMY_SINK_RECEIVE_BUFFER_SIZE 100000

DummySink* DummySink::createNew(UsageEnvironment& env, std::function<void(HyFrame*)> recvCallback, MediaSubsession& subsession, char const* streamId) {
	sendRecvFrameOnSink = recvCallback;

	return new DummySink(env, subsession, streamId);
}

DummySink::DummySink(UsageEnvironment& env, MediaSubsession& subsession, char const* streamId)
	: MediaSink(env),
	fSubsession(subsession) {
	fStreamId = strDup(streamId);
	fReceiveBuffer = new u_int8_t[DUMMY_SINK_RECEIVE_BUFFER_SIZE];

#pragma region Added from demoLig555WithFFMPEG Open Source
	fReceiveBufferAV = new u_int8_t[DUMMY_SINK_RECEIVE_BUFFER_SIZE + 4];
	fReceiveBufferAV[0] = 0;
	fReceiveBufferAV[1] = 0;
	fReceiveBufferAV[2] = 0;
	fReceiveBufferAV[3] = 1;

	av_init_packet(&avpkt);
	avpkt.flags |= AV_PKT_FLAG_KEY;
	avpkt.pts = avpkt.dts = 0;

	/* set end of buffer to 0 (this ensures that no overreading happens for damaged mpeg streams) */
	memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);

	//codec = avcodec_find_decoder(CODEC_ID_MPEG1VIDEO);
	codec = avcodec_find_decoder(AV_CODEC_ID_H264);
	if (!codec) {
		envir() << "codec not found!";
		exit(4);
	}

	codecContext = avcodec_alloc_context3(codec);
	picture = av_frame_alloc();

	int output_size = av_image_get_buffer_size(AV_PIX_FMT_BGR24, 640, 480, 1);
	uint8_t* buffer = (uint8_t *)av_malloc(output_size * sizeof(uint8_t));

	frameRGB = av_frame_alloc();
	av_image_fill_arrays(frameRGB->data, frameRGB->linesize, buffer, AV_PIX_FMT_BGR24, 640, 480, 1);
	frameRGB->format = AV_PIX_FMT_BGR24;
	frameRGB->width = 640;
	frameRGB->height = 480;

	if (codec->capabilities & AV_CODEC_CAP_TRUNCATED) {
		codecContext->flags |= AV_CODEC_FLAG_TRUNCATED; // we do not send complete frames
	}

	codecContext->width = 1920;
	codecContext->height = 1080;
	codecContext->pix_fmt = AV_PIX_FMT_YUV420P;

	/* for some codecs width and height MUST be initialized there because this info is not available in the bitstream */

	if (avcodec_open2(codecContext, codec, NULL) < 0) {
		envir() << "could not open codec";
		exit(5);
	}

	//	struct SwsContext *sws;
	//	sws = sws_getContext(
	//		codecContext->width,
	//		codecContext->height,
	//		AV_PIX_FMT_YUV420P,
	//		codecContext->width,
	//		codecContext->height,
	//		AV_PIX_FMT_YUV420P,
	//		SWS_BICUBIC,
	//		NULL,
	//		NULL,
	//		NULL
	//	);
	//	sws_scale(
	//		sws,
	//		picture->data,
	//		picture->linesize,
	//		0,
	//		codecContext->height,
	//		pict.data,
	//		pict.linesize
	//	);

	//convertContext = sws_getContext(codecContext->width, codecContext->height, codecContext->pix_fmt,
	//	640, 480, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);

	//sws_scale(convertContext, picture->data, picture->linesize, 0, codecContext->height, frameRGB->data, frameRGB->linesize);
	#pragma endregion

	frameCnt = 0;
}

DummySink::~DummySink() {
	delete[] fReceiveBuffer;
	delete[] fStreamId;
}

void DummySink::afterGettingFrame(void* clientData, unsigned frameSize, unsigned numTruncatedBytes,
	struct timeval presentationTime, unsigned durationInMicroseconds) {
	DummySink* sink = (DummySink*)clientData;

	sink->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime, durationInMicroseconds);
}

// If you don't want to see debugging output for each received frame, then comment out the following line:
//#define DEBUG_PRINT_EACH_RECEIVED_FRAME

void DummySink::afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
	struct timeval presentationTime, unsigned /*durationInMicroseconds*/) {
	// We've just received a frame of data.  (Optionally) print out information about it:
#ifdef DEBUG_PRINT_EACH_RECEIVED_FRAME
	if (fStreamId != NULL) envir() << "Stream \"" << fStreamId << "\"; ";
	envir() << fSubsession.mediumName() << "/" << fSubsession.codecName() << ":\tReceived " << frameSize << " bytes";
	if (numTruncatedBytes > 0) envir() << " (with " << numTruncatedBytes << " bytes truncated)";
	char uSecsStr[6 + 1]; // used to output the 'microseconds' part of the presentation time
	sprintf(uSecsStr, "%06u", (unsigned)presentationTime.tv_usec);
	envir() << ".\tPresentation time: " << (int)presentationTime.tv_sec << "." << uSecsStr;
	if (fSubsession.rtpSource() != NULL && !fSubsession.rtpSource()->hasBeenSynchronizedUsingRTCP()) {
		envir() << "!"; // mark the debugging output to indicate that this presentation time is not RTCP-synchronized
	}
#ifdef DEBUG_PRINT_NPT
	envir() << "\tNPT: " << fSubsession.getNormalPlayTime(presentationTime);
#endif
	envir() << "\n";
#endif

#pragma region Added from demoLive555WithFFMPEG Open Source
	printf("CODEC_NAME : %s\n", fSubsession.codecName());

	if (strcmp(fSubsession.codecName(), "H264") == 0) {
		avpkt.data = fReceiveBufferAV;
		//	r2sprop();
		//	r2sprop2();
		//	avpkt.size = (int)fReceiveBuffer[0];
		avpkt.size = frameSize + 4;
		//	avpkt.size = frameSize;
		if (avpkt.size != 0) {
			memcpy(fReceiveBufferAV + 4, fReceiveBuffer, frameSize);
			avpkt.data = fReceiveBufferAV; //+2;


			int result = avcodec_send_packet(codecContext, &avpkt);

			char* strerr = new char[4096];
			av_strerror(result, strerr, 4096);

			printf("%s\n", strerr);

			if (result < 0) 
			{
				continuePlaying();
				return; 
			}

			result = avcodec_receive_frame(codecContext, picture);
			if (result < 0) 
			{
				continuePlaying();
				return; 
			}

			// set source width, height

			// 640,480 - destination size
			int byte_size = av_image_get_buffer_size(AV_PIX_FMT_BGR24, 640, 480, 1);

			if (convertContext == nullptr)
			{
				convertContext = sws_getContext(codecContext->width, codecContext->height, codecContext->pix_fmt,
					640, 480, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);
			}

			sws_scale(convertContext, picture->data, picture->linesize, 0, codecContext->height, frameRGB->data, frameRGB->linesize);

			HyFrame *frame = new HyFrame();
			frame->data = new uint8_t[byte_size * sizeof(uint8_t)];
			memset(frame->data, 0x00, byte_size * sizeof(uint8_t));

			memcpy(frame->data, picture->data[0], byte_size * sizeof(uint8_t));

			frame->org_width = codecContext->width;
			frame->org_height = codecContext->height;
			frame->data_size = byte_size * sizeof(uint8_t);
			frame->frame_id = frameCnt++;
			frame->width = 640;
			frame->height = 480;
			frame->pitch = frameRGB->linesize[0];
			frame->inter_frame_delay = codecContext->ticks_per_frame * 1000 * codecContext->time_base.num / codecContext->time_base.den;

			sendRecvFrameOnSink(frame);


										   //		avpkt.data = fReceiveBuffer; //+2;
			//len = avcodec_decode_video2(codecContext, picture, &got_picture, &avpkt);
			//if (len < 0) {
			//	envir() << "Error while decoding frame" << frame;
			//	//			exit(6);
			//}
			//if (got_picture) {

			//	// do something with it
			//	//SDL_LockYUVOverlay(bmp);

			//	/*AVPicture pict;
			//	pict.data[0] = bmp->pixels[0];
			//	pict.data[1] = bmp->pixels[2];
			//	pict.data[2] = bmp->pixels[1];

			//	pict.linesize[0] = bmp->pitches[0];
			//	pict.linesize[1] = bmp->pitches[2];
			//	pict.linesize[2] = bmp->pitches[1];*/

			//	AVFrame frameRGB

			//	struct SwsContext *sws;
			//	sws = sws_getContext(
			//		codecContext->width,
			//		codecContext->height,
			//		AV_PIX_FMT_YUV420P,
			//		codecContext->width,
			//		codecContext->height,
			//		AV_PIX_FMT_YUV420P,
			//		SWS_BICUBIC,
			//		NULL,
			//		NULL,
			//		NULL
			//	);
			//	sws_scale(
			//		sws,
			//		picture->data,
			//		picture->linesize,
			//		0,
			//		codecContext->height,
			//		pict.data,
			//		pict.linesize
			//	);



			//	//SDL_UnlockYUVOverlay(bmp);

			//	//rect.x = 0;
			//	//rect.y = 0;
			//	//rect.w = c->width;
			//	//rect.h = c->height;
			//	//SDL_DisplayYUVOverlay(bmp, &rect);


			//	//
			//	/*
			//	char fname[256]={0};
			//	sprintf(fname, "OriginalYUV%d.pgm",frame);
			//	pgm_save (
			//	picture->data[0],
			//	picture->linesize[0],
			//	c->width,
			//	c->height,
			//	fname
			//	);
			//	*/
			//	//sws_freeContext(sws);
			//	frame++;
			//}
			//else {
			//	envir() << "no picture :( !\n";
			//}
		}
	}
#pragma endregion

	// Then continue, to request the next frame of data:
	continuePlaying();
}

Boolean DummySink::continuePlaying() {
	if (fSource == NULL) return False; // sanity check (should not happen)

									   // Request the next frame of data from our input source.  "afterGettingFrame()" will get called later, when it arrives:
	fSource->getNextFrame(fReceiveBuffer, DUMMY_SINK_RECEIVE_BUFFER_SIZE,
		afterGettingFrame, this,
		onSourceClosure, this);
	return True;
}
