#include "stdafx.h"
#include "KeyboardHook.h"
#include "LayeredWindow.h"


KeyboardHook::KeyboardHook()
{
}


KeyboardHook::~KeyboardHook()
{
	UnhookWindowsHookEx(hHook);
}

bool KeyboardHook::Install()
{
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT
	{
		if (LayeredWindow::lastWindow && nCode >= 0)
			if (SendMessage(LayeredWindow::lastWindow->handle, WM_USER + 200, wParam, lParam))
				return true;

		return ::CallNextHookEx(nullptr, nCode, wParam, lParam);
	}, nullptr, NULL);
	return true;
}