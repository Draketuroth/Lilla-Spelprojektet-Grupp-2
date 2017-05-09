#pragma once
#include <windows.h>




class Timer
{
private:
	__int64 countsPerSecond;
	float secondsPerCount;

	__int64 previousTime;
	__int64 currentTime;

	float deltaTime;
	int frameCount;
public:
	Timer();
	~Timer();

	void initialize();
	float getDeltaTime();
	float getCurrentTime();
	void updateCurrentTime();
	void resetTimer();
	int getFrameCount();
	float getCountsPerSecond();
};

