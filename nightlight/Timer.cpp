#include "Timer.h"

using namespace std;

Timer::Timer()
{
	// Check to see if this system supports high performance timers
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);

	if (frequency == 0)
	{
		throw runtime_error("Could not initialize Timer");
	}

	ticksPerMs = (float)(frequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	gameTime = 0;
}

Timer::~Timer()
{
}

void Timer::Update()
{
	INT64 currentTime;
	double timeDifference;

	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);		//Query the current time
	timeDifference = (double)(currentTime - startTime); 		//Difference in time since the last time-query 
	frameTime = timeDifference / ticksPerMs;					//Time difference over the timer speed resolution give frameTime
	startTime = currentTime; 									//Restart the timer

	gameTime += frameTime;

	framesPerSecond.Update();
}

double Timer::GetFrameTime()
{
	return frameTime;
}

double Timer::GetGameTime()
{
	return gameTime;
}

int Timer::GetFPS()
{
	return framesPerSecond.fps;
}

void Timer::Reset()
{
	gameTime = 0;
}

