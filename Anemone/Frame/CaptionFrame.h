#pragma once
#include "BaseFrame.h"

class CaptionFrame : public BaseFrame
{
	bool bBypass;
	bool bPauseKBHook;

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
	virtual bool OnKeyboardHookProc(WPARAM wParam, LPARAM lParam);
};

