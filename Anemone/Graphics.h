#pragma once
#include "SkGraphics.h"
#include "SkFontMgr.h"
#include "SkCanvas.h"
#include "SkColor.h"
#include "SkAutoMalloc.h"
#include "SkTypeface.h"
#include "SkTextBlob.h"
#include "SkGradientShader.h"
#include "SkMaskFilter.h"
#include "SkColorFilter.h"
#include "SkBlurMask.h"
#include "DisplayParams.h"

namespace Graphics
{
	void Init();
	auto GetGraphicsFromContext();
	void SwapBuffer(HWND hWnd, int fWidth, int fHeight);
	void CreateContext(HWND hWnd, int fWidth, int fHeight);
	void Resize(HWND hWnd, int fWidth, int fHeight);

	void PaintLoop(HWND hWnd);
	void DrawSysMenu(HWND hWnd, SkBitmap &bitmap);

	bool IsCursorInMinimize(HWND hWnd);
	bool IsCursorInClose(HWND hWnd);
	SkRect GetMinimizeRect(HWND hWnd);
	SkRect GetCloseRect(HWND hWnd);
}

