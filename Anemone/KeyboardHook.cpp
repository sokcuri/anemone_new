#include "stdafx.h"
#include "KeyboardHook.h"


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
		if (lastWindow && nCode >= 0)
			if (SendMessage(lastWindow->handle, WM_USER + 200, wParam, lParam))
				return true;

		return ::CallNextHookEx(nullptr, nCode, wParam, lParam);
	}, nullptr, NULL);
	return true;
}