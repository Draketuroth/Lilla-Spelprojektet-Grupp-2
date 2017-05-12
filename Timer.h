#pragma once
#include <windows.h>
#include <time.h>



class Timer
{
private:
	__int64 countsPerSecond;
	float secondsPerCount;

	__int64 previousTime;
	__int64 currentTime;

	float deltaTime;
	int frameCount;

	//to Lava movment
	
public:
	Timer();
	~Timer();
	//to Lava movment
	int getSeconds(); 
	void initialize();
	float getDeltaTime();
	float getCurrentTime();
	void updateCurrentTime();
	void resetTimer();
	int getFrameCount();
	float getCountsPerSecond();
};

