#pragma once
#include "BaseFrame.h"

class CaptionFrame : public BaseFrame
{
	bool bBypass;
	bool bPauseKBHook;
	int n_selLine;

	size_t typeNum;
	time_point lastTime;

public:
	CaptionFrame();
	~CaptionFrame();
	bool BrowseFile();
	void DrawLineCount();

	virtual bool OnFirstProc();
	virtual bool OnRender();
	virtual bool OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);
};

