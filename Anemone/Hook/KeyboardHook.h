#pragma once
#include "BaseHook.h"

class KeyboardHook : public BaseHook
{
public:
	KeyboardHook();
	~KeyboardHook();
	
	bool Install();
};

