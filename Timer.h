
#include <windows.h>




class Timer
{
private:
	__int64 countsPerSecond;
	float secondsPerCount;

	__int64 previousTime;
	__int64 currentTime;

	float deltaTime;

public:
	Timer();
	~Timer();

	void initialize();
	float getDeltaTime();
	void updateCurrentTime();
	void resetTimer();
};

