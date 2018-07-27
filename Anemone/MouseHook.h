#pragma once
#include "BaseHook.h"

class MouseHook : public BaseHook
{
public:
	MouseHook();
	~MouseHook();

	virtual bool Install();
};

