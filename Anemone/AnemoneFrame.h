#pragma once
#include "LayeredWindow.h"
#include "SettingDlg.h"
#include "SKRRect.h"
#include "SKString.h"
#include "SkFontMgr.h"
#include "SkTypeface.h"
#include "SkMaskFilter.h"
#include "SkBlurMask.h"

class AnemoneFrame : public LayeredWindow
{
public:
	AnemoneFrame();
	~AnemoneFrame();

	virtual void OnRender();
	RawSize GetWindowSize();
	void DrawSysMenu();
	void DrawContent();

	virtual bool OnCommand(LPARAM lParam, WPARAM wParam);

	virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

