#pragma once
#include "BaseFrame.h"

using system_clock = std::chrono::system_clock;
using time_point = system_clock::time_point;
using milli_duration = std::chrono::duration<double, std::milli>;

class ClipboardFrame : public BaseFrame
{
	bool bBypass;
	bool bFirst;
	bool bStart;

	HWND hwndNextViewer;
	size_t typeNum;

	time_point lastTime;

public:
	ClipboardFrame();
	~ClipboardFrame();

	virtual bool OnRender();
	virtual bool OnCreate();
	virtual bool OnChangeCbChain(WPARAM wParam, LPARAM lParam);
	virtual bool OnDrawClipboard();
	virtual bool OnDestroy();
};

