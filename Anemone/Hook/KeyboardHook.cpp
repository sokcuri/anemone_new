/**
* This file is part of Anemone.
*
* Anemone is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* The Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Anemone is distributed in the hope that it will be useful,
*
* But WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Anemone.
*
*   If not, see <http://www.gnu.org/licenses/>.
*
**/

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