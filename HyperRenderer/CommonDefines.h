#pragma once
//#include <functional>
#include "stdint.h"
#include "HyFrame.h"

typedef void(*recvHandler)(HyFrame* recvFrame);
//std::function<void(HyFrame*)> recvHandler;
