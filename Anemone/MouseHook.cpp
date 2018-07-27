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
