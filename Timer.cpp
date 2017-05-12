#include "Timer.h"



Timer::Timer()
{
	this->countsPerSecond = 0;
	this->secondsPerCount = 0;

	this->previousTime = 0;
	this->currentTime = 0;

	this->deltaTime;
	this->frameCount = 0;
}
Timer::~Timer()
{
}

void Timer::initialize()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&this->countsPerSecond);
	this->secondsPerCount = 1.0f / this->countsPerSecond;

	QueryPerformanceCounter((LARGE_INTEGER*)&this->previousTime);
}

float Timer::getDeltaTime()
{
	this->currentTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&this->currentTime);

	this->deltaTime = ((this->currentTime - previousTime) * this->secondsPerCount);
	return this->deltaTime;
}

float Timer::getCurrentTime()
{
	return this->currentTime;
}

void Timer::updateCurrentTime()
{
	this->previousTime = this->currentTime;
	
	
	if (this->frameCount > 4000)
	{
		this->frameCount = 0;
	}
	this->frameCount += 880 * getDeltaTime();
}

void Timer::resetTimer()
{

}
int Timer::getFrameCount()
{
	return this->frameCount;
}

float Timer::getCountsPerSecond()
{
	return this->countsPerSecond;
}

int Timer::getSeconds()
{
	time_t timer; 
	int seconds; 

	//timer(&seconds);

	//
	//if (seconds > 12)
	//{
	//	seconds = 0; 
	//}

	return seconds; 
}
