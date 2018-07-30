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

	std::wstring advTitle;
	std::wstring strText;

	RawSize GetWindowSize();
	void DrawSysMenu();
	void DrawContent();

	virtual bool OnCommand(WPARAM wParam, LPARAM lParam);

	//virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

