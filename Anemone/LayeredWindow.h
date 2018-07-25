#pragma once
#include "Anemone.h"
#include "resource.h"
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
	WNDCLASSEXW wcex;

	FPSCounter fps;

	HANDLE thread;

	bool onMouse = false;
	RECT mouseRect;

	HANDLE m_hMHThread;
	HANDLE m_hKHThread;
	HHOOK m_hKeyboardHook;
	HHOOK m_hMouseHook;

	static int m_nMode;
public:
	HWND handle;
	LayeredBuffer *buffer;
	LayeredContext *context;

	LayeredWindow();
	~LayeredWindow();

	void Create();
	void Resize(int width, int height);
	virtual void OnRender();
	virtual bool OnCreate();
	virtual bool OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	void Initialize();

	static LRESULT _WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

