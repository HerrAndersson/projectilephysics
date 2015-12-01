#pragma once
#include <windows.h>
#include <stdexcept>

class Timer
{
private:

	INT64 frequency;
	double ticksPerMs;
	INT64 startTime;

	double tempTime;
	double frameTime;   //Time since last frame
	double gameTime;	//Time since the beginning of the game

public:

	Timer();
	~Timer();

	void Update();

	double GetFrameTime();
	double GetGameTime();

	int GetFPS();

	void Reset();

};

