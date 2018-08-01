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
#include "BaseFrame.h"

class HookerFrame : public BaseFrame
{
	bool bBypass;
	bool bPauseKBHook;

	size_t typeNum;
	time_point lastTime;

	HANDLE hHookThread;
public:
	HookerFrame();
	~HookerFrame();

	static INT_PTR CALLBACK HookerStickProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	bool BrowseFile();
	void DrawLineCount();

	virtual bool OnFirstProc(HWND hWnd);
	virtual bool OnRender();
	virtual bool OnCommand(WPARAM wParam, LPARAM lParam);
	virtual bool OnKeyboardHookProc(WPARAM wParam, LPARAM lParam);
};

