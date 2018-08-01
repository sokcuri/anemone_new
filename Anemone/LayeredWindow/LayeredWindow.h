#pragma once
#include "Anemone.h"
#include "resource.h"
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

#include "RawPoint.h"
#include "RawSize.h"
#include "LayeredBuffer.h"
#include "LayeredContext.h"
#include "FPSCounter.h"
#include <functional>

class LayeredWindow
{
	CRITICAL_SECTION cs;
	std::wstring title;
	FPSCounter fps;
	HANDLE thread;
	bool onMouse = false;
	RECT mouseRect;
public:
	static LayeredWindow *lastWindow;

	HWND handle;
	int n_selLine;
	std::vector<std::wstring> vecBuff;

	LayeredWindow();
	virtual ~LayeredWindow();

	bool Create();
	void Resize(int width, int height);
	virtual bool OnRender();
	virtual bool OnCreate();
	virtual bool OnFirstProc(HWND hWnd);
	virtual bool OnCommand(WPARAM wParam, LPARAM lParam);
	virtual bool OnDestroy();
	virtual bool OnChangeCbChain(WPARAM wParam, LPARAM lParam);
	virtual bool OnDrawClipboard();
	virtual bool OnDestroyClipboard();
	virtual bool OnKeyboardHookProc(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);

protected:
	RawPoint position;
	RawSize size;

	LayeredBuffer *buffer;
	LayeredContext *context;

	int m_mode = 0;
	WNDCLASSEXW wcex;


	std::wstring strBuff;

private:
	void Initialize();
	void StartEventCapture();
	static LRESULT _WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

