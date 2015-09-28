#pragma once
#include <Pdh.h>
#include <pdhmsg.h>
#pragma comment(lib, "pdh.lib")

class Cpu
{
private:

	bool canReadCpu;
	HQUERY queryHandle;
	HCOUNTER counterHandle;

	ULONGLONG lastSampleTime;
	long cpu;

public:

	Cpu();
	~Cpu();

	void Update();
	int GetCpuPercentage();
};

