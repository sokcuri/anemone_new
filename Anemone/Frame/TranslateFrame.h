#pragma once
#include "BaseFrame.h"

class TranslateFrame : public BaseFrame
{
public:
	TranslateFrame();
	~TranslateFrame();

	virtual bool OnRender();
};

