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
#include "MouseHook.h"

MouseHook::MouseHook()
{
}

MouseHook::~MouseHook()
{
	UnhookWindowsHookEx(hHook);
}

bool MouseHook::Install()
{
	return true;
	hHook = SetWindowsHookEx(WH_MOUSE_LL, [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT
	{
		if (nCode == HC_ACTION)
		{
			POINT *lp = (POINT *)lParam;

			HWND hMenuWnd = FindWindow(L"#32768", L"AnemoneContextMenu");
			DWORD processId;
			DWORD actualProcId = GetWindowThreadProcessId(hMenuWnd, &processId);

			if (hMenuWnd && processId == GetCurrentProcessId())
			{
				RECT r;
				GetWindowRect(hMenuWnd, &r);
				if (!(lp->x >= r.left && lp->x <= r.right && lp->y >= r.top && lp->y <= r.bottom))
				{
					if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN)
					{
						ShowWindow(hMenuWnd, false);
						CloseWindow(hMenuWnd);
					}
				}
			}
		}
		return ::CallNextHookEx(nullptr, nCode, wParam, lParam);
	}, nullptr, NULL);
	return true;
}
