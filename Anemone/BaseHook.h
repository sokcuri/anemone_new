#pragma once
class BaseHook
{
public:
	HHOOK hHook;
	BaseHook();
	~BaseHook();

	virtual bool Install() = 0;
};

