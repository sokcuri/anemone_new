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
#include "LayeredWindow.h"
#include "SKRRect.h"
#include "SKString.h"
#include "SkFontMgr.h"
#include "SkTypeface.h"
#include "SkMaskFilter.h"
#include "SkBlurMask.h"

#define CLIPBOARD_MODE	1
#define CAPTION_MODE	2
#define TRANSLATE_MODE	3

class BaseFrame : public LayeredWindow
{
public:
	BaseFrame();
	~BaseFrame();

	std::wstring m_title;
	std::wstring strText;

	RawSize GetWindowSize();
	void DrawSysMenu();
	void DrawContent();

	virtual bool OnCommand(WPARAM wParam, LPARAM lParam);

	//virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

