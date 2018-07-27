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

	std::wstring title;

	FPSCounter fps;
	HANDLE thread;

	bool onMouse = false;
	RECT mouseRect;
public:
	RawPoint position;
	RawSize size;

	WNDCLASSEXW wcex;

	HWND handle;
	LayeredBuffer *buffer;
	LayeredContext *context;

	std::vector<std::wstring> vecBuff;
	int n_selLine;

	std::wstring strBuff;

	LayeredWindow();
	~LayeredWindow();

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
	int m_mode = 0;

private:
	void Initialize();

	void StartEventCapture();

	static LRESULT _WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

