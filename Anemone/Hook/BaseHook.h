#pragma once
class BaseHook
{
public:
	HHOOK hHook;
	BaseHook();
	virtual ~BaseHook();

	virtual bool Install() = 0;
};

