#pragma once
#include "BaseFrame.h"

class CaptionFrame : public BaseFrame
{
public:
	CaptionFrame();
	~CaptionFrame();
	virtual bool OnFirstProc();
	virtual bool OnRender();
};

