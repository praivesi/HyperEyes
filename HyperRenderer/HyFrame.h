#pragma once
#include <cstdint>

class HyFrame {
public:
	uint8_t *data;
	int org_width;
	int org_height;
	int data_size;
	int width;
	int height;
	int pitch;
	int frame_id;
	int inter_frame_delay;

public:
	HyFrame() : data(nullptr) {};
	~HyFrame() {
		if (data != nullptr)
		{
			delete data;
			data = nullptr;
		}
	}
};
