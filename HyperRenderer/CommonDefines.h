#pragma once
#include "stdint.h"

typedef void(*recvHandler)(uint8_t* pReceiveBuffer, char* pStreamId, unsigned frameSize, unsigned numTruncatedBytes,
	struct timeval presentationTime, unsigned durationInMicroseconds);
