#pragma once
#include <windows.h>
#include <time.h>
#include <chrono>
#include <iostream>
using namespace std;
using namespace chrono;

class Timer
{
private:
	__int64 countsPerSecond;
	float secondsPerCount;

	__int64 previousTime;
	__int64 currentTime;

	float deltaTime;
	int frameCount;
	float secondCounter;
	//to Lava movment
	
public:
	Timer();
	~Timer();
	//to Lava movment
	float getSeconds(); 
	void initialize();
	float getDeltaTime();
	float getCurrentTime();
	void updateCurrentTime();
	void resetTimer();
	int getFrameCount();
	float getCountsPerSecond();
};

