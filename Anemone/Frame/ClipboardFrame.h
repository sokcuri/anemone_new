#pragma once
#include "BaseFrame.h"

class ClipboardFrame : public BaseFrame
{
public:
	ClipboardFrame();
	~ClipboardFrame();

	virtual bool OnRender();
};

