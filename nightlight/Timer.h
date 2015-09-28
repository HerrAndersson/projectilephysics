#pragma once
#include <windows.h>
#include <stdexcept>

class Timer
{
private:

	struct FPS
	{
		int fps;
		int count;
		INT64 startTime;

		FPS()
		{
			fps = 0;
			count = 0;
			INT64 currentTime;
			QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
			startTime = currentTime;
		}
		void Update()
		{
			count++;

			INT64 currentTime;
			QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

			if (currentTime >= (startTime + 1000))
			{
				fps = count;
				count = 0;
				startTime = currentTime;
			}
		}
	};

	INT64 frequency;
	double ticksPerMs;
	INT64 startTime;

	double frameTime; //Time since last frame
	double gameTime;  //Time since the beginning of the game

	FPS framesPerSecond;

public:

	Timer();
	~Timer();

	void Update();

	double GetFrameTime();
	double GetGameTime();
	int GetFPS();

	void Reset();

};

