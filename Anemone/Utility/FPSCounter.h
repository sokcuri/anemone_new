#pragma once
#include <chrono>
using system_clock = std::chrono::system_clock;
using time_point = std::chrono::system_clock::time_point;
using milli_duration = std::chrono::duration<double, std::milli>;

class FPSCounter
{
	int fpsStep = 0;
	time_point lastTime;

public:
	int lastFps = 0;

	FPSCounter();
	~FPSCounter();
	void Update();

	std::string ToString();
};

