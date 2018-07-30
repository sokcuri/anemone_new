/**
* This file is part of Anemone.
*
* Anemone is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* The Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Anemone is distributed in the hope that it will be useful,
*
* But WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Anemone.
*
*   If not, see <http://www.gnu.org/licenses/>.
*
**/

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

