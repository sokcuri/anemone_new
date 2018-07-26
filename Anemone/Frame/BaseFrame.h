#pragma once
#include "..\SettingDlg.h"
#include "LayeredWindow.h"
#include "SKRRect.h"
#include "SKString.h"
#include "SkFontMgr.h"
#include "SkTypeface.h"
#include "SkMaskFilter.h"
#include "SkBlurMask.h"

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

