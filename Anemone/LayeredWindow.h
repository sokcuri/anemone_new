#pragma once
#include "RawPoint.h"
#include "RawSize.h"
#include "LayeredBuffer.h"
#include "LayeredContext.h"
#include "FPSCounter.h"

class LayeredWindow
{
	RawPoint position;
	RawSize size;

	std::wstring title;
	WNDCLASSEXW wcex;

	FPSCounter fps;

public:
	HWND handle;
	LayeredBuffer *buffer;
	LayeredContext *context;

	LayeredWindow();
	~LayeredWindow();

	void Create(bool bShow);
	void Resize(int width, int height);
	virtual void OnRender();
private:
	void Initialize();

	static LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};

