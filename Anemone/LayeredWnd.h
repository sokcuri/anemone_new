#pragma once
#include "SkRect.h"
namespace LayeredWnd
{
	LRESULT CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnPaint(HWND hWnd);
	LRESULT CALLBACK OnSize(HWND hWnd, int width, int height);
}

