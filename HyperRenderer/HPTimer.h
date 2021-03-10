#pragma once
#ifndef HP_TIMER_H
#define HP_TIMER_H

#pragma once

#include <Windows.h>

class HPTimer {
public:
	HPTimer();
	~HPTimer();

	void Reset();
	void Update();

	double GetTimeTotal();
	double GetTimeDelta();

private:
	long long m_start_time;
	long long m_last_call_to_update;
	long long m_current_call_to_update;
	long long m_frequency;
};

#endif // HP_TIMER_H

