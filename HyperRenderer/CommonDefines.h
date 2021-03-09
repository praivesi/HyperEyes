#pragma once

typedef void(*recvHandler)(unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime);
