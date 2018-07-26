#pragma once
#include "BaseFrame.h"

class CaptionFrame : public BaseFrame
{
	int n_selLine;

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

