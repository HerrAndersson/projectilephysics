#pragma once
#include <windows.h>
#include <stdexcept>

class Timer
{
private:

	INT64 frequency;
	double ticksPerMs;
	INT64 startTime;

	double tempTime;   //Time since last frame
	double frameTime;  //Time since the beginning of the game
	double gameTime;

public:

	Timer();
	~Timer();

	void Update();

	double GetFrameTime();
	double GetGameTime();

	void Reset();

};

