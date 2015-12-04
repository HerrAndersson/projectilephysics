#include "Timer.h"

using namespace std;

Timer::Timer()
{
	// Check to see if this system supports high performance timers
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);

	if (frequency == 0)
	{
		throw runtime_error("Timer: Could not initialize Timer");
	}

	ticksPerMs = (float)(frequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	frameTime = 0;
	gameTime = 0;
}

Timer::~Timer()
{
}

void Timer::Update()
{
	INT64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);		//Query the current time
	double timeDifference = (double)(currentTime - startTime); 	//Difference in time since the last time-query 
	tempTime = timeDifference / ticksPerMs;						//Time difference over the timer speed resolution give frameTime
	startTime = currentTime; 									//Restart the timer

	frameTime += tempTime;
	gameTime += tempTime;

	if (gameTime > (DBL_MAX - 100000000))
		gameTime = 0;
}

double Timer::GetFrameTime()
{
	return frameTime;
}

double Timer::GetGameTime()
{
	return gameTime;
}

void Timer::Reset()
{
	frameTime = 0;
}

int Timer::GetFPS()
{
	return (int)(1 / (frameTime / 1000));
}

