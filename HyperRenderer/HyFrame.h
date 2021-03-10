#pragma once

#include <cstdint>
#include "CommonFunctions.h"

class HyFrame {
public:
	uint8_t * data;
	int original_width;
	int original_height;
	int data_size;
	int width;
	int height;
	int pitch;
	int frame_id;
	int inter_frame_delay;

	HyFrame() : data(nullptr) {};

	~HyFrame() {
		SafeRelease(data);
	}
};