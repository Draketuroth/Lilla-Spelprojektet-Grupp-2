#include "Timer.h"



Timer::Timer()
{
	this->countsPerSecond = 0;
	this->secondsPerCount = 0;

	this->previousTime = 0;
	this->currentTime = 0;

	this->deltaTime;
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
	QueryPerformanceCounter((LARGE_INTEGER*)&this->currentTime);

	this->deltaTime = ((this->currentTime - previousTime) * this->secondsPerCount);
	return this->deltaTime;
}

void Timer::updateCurretTime()
{
	this->previousTime = this->currentTime;
}