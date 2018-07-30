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
	std::wstring title;
	FPSCounter fps;
	HANDLE thread;
	bool onMouse = false;
	RECT mouseRect;
public:
	static LayeredWindow *lastWindow;

	HWND handle;
	int n_selLine;
	std::vector<std::wstring> vecBuff;

	LayeredWindow();
	virtual ~LayeredWindow();

	bool Create();
	void Resize(int width, int height);
	virtual bool OnRender();
	void EndEventCapture();
	virtual bool OnCreate();
	virtual bool OnFirstProc();
	virtual bool OnCommand(WPARAM wParam, LPARAM lParam);
	virtual bool OnDestroy();
	virtual bool OnChangeCbChain(WPARAM wParam, LPARAM lParam);
	virtual bool OnDrawClipboard();
	virtual bool OnDestroyClipboard();
	virtual bool OnKeyboardHookProc(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);

protected:
	RawPoint position;
	RawSize size;

	LayeredBuffer *buffer;
	LayeredContext *context;

	int m_mode = 0;
	WNDCLASSEXW wcex;


	std::wstring strBuff;

private:
	void Initialize();
	void StartEventCapture();
	static LRESULT _WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

