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

#include "stdafx.h"
#include "FPSCounter.h"


FPSCounter::FPSCounter()
{
	lastTime = system_clock::now();
}

FPSCounter::~FPSCounter()
{
}

void FPSCounter::Update()
{
	fpsStep++;
	milli_duration duration = system_clock::now() - lastTime;
	if (duration.count() > 1000.0f)
	{
		lastTime = system_clock::now();
		lastFps = fpsStep;
		fpsStep = 0;
	}
}

std::string FPSCounter::ToString()
{
	return fmt::format("{0}fps", lastFps);
}

