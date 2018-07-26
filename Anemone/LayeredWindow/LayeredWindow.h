#pragma once
#include "../Anemone.h"
#include "../resource.h"
#include "RawPoint.h"
#include "RawSize.h"
#include "LayeredBuffer.h"
#include "LayeredContext.h"
#include "FPSCounter.h"
#include <functional>

class LayeredWindow
{
	CRITICAL_SECTION cs;

	RawPoint position;
	RawSize size;

	std::wstring title;

	FPSCounter fps;

	HANDLE thread;

	bool onMouse = false;
	RECT mouseRect;

	HANDLE m_hMHThread;
	HANDLE m_hKHThread;
	HHOOK m_hKeyboardHook;
	HHOOK m_hMouseHook;

public:
	WNDCLASSEXW wcex;

	HWND handle;
	LayeredBuffer *buffer;
	LayeredContext *context;

	LayeredWindow();
	~LayeredWindow();

	void Create();
	void Resize(int width, int height);
	virtual bool OnRender();
	virtual bool OnCreate();
	virtual bool OnFirstProc();
	virtual bool OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
	int m_mode = 0;

private:
	void Initialize();

	static LRESULT _WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

