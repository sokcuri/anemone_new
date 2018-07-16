#include "stdafx.h"
#include "LayeredWnd.h"
#include "Anemone.h"
#include "resource.h"
#include "Graphics.h"
#include "SettingDlg.h"

namespace LayeredWnd
{
	UINT latest_evt;
	bool bMinimizeClk = false;
	bool bCloseClk = false;
	HHOOK m_hCBTHook;
	HHOOK m_hMouseHook;
	HWND m_hMenuWnd;
	HANDLE m_hMHThread;
	HANDLE m_hDrawThread;

	LRESULT CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_CREATE:
		{
			DWORD dwThreadID = NULL;

			m_hMHThread = (HANDLE)_beginthreadex(NULL, 0, [](void* pData) -> unsigned int {
				static HWND m_hWnd;
				m_hWnd = (HWND)pData;
				m_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT
				{
					if (nCode == HC_ACTION)
					{
						POINT *lp = (POINT *)lParam;

						if (IsWindow(m_hMenuWnd))
						{
							RECT r;
							GetWindowRect(m_hMenuWnd, &r);
							if (!(lp->x >= r.left && lp->x <= r.right && lp->y >= r.top && lp->y <= r.bottom))
							{
								PostMessage(m_hWnd, wParam, 0, 0); // WM_LBUTTONDOWN or WM_RBUTTONDOWN
								PostMessage(m_hWnd, WM_MOUSEMOVE, 0, 0);
							}
						}
					}
					return ::CallNextHookEx(m_hMouseHook, nCode, wParam, lParam);
				}, g_hInst, NULL);
				if (!m_hMouseHook)
					MessageBox(0, L"Mouse hook failed", 0, 0);

				MSG msg;
				while (GetMessage(&msg, NULL, 0, 0))
				{
					if (!TranslateAccelerator(msg.hwnd, 0, &msg))
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				}
				return (int)msg.wParam; 
			}, (void *)hWnd, 0, (unsigned*)&dwThreadID);

			if (m_hMHThread == 0) MessageBox(0, L"MenuThread Error\n", 0, 0);

			m_hDrawThread = (HANDLE)_beginthreadex(NULL, 0, [](void *args) -> unsigned int {
				HWND hWnd = (HWND)args;
				int wait = 0;
				std::chrono::system_clock::time_point a = std::chrono::system_clock::now();
				std::chrono::system_clock::time_point b = std::chrono::system_clock::now();
				int old_opacity;
				while (IsWindow(hWnd))
				{
					a = std::chrono::system_clock::now();
					old_opacity = guide_opacity;
					if (mouse_evt && guide_opacity == 255)
					{
						Sleep(10);
					}
					else if (mouse_evt)
					{
						while (guide_opacity < 255)
						{
							if (!mouse_evt) break;

							b = std::chrono::system_clock::now();
							std::chrono::duration<double, std::milli> elapsed_time = b - a;
							if (!elapsed_time.count()) break;

							float pre_progressed = (float)old_opacity / 256;
							float progress = ((float)elapsed_time.count() / 200.0f) + pre_progressed;

							if (256 * progress > 256) guide_opacity = 255;
							else if ((int)(256 * progress) == 0) continue;
							else guide_opacity = (int)(256 * progress);
							SendMessage(hWnd, WM_PAINT, 0, 0);

							OutputDebugString(L"A");
						}
					}
					else if (guide_opacity != 1)
					{
						while (guide_opacity > 1)
						{
							if (mouse_evt) break;

							b = std::chrono::system_clock::now();
							std::chrono::duration<double, std::milli> elapsed_time = b - a;
							if (!elapsed_time.count()) break;

							float pre_progressed = 1 - ((float)old_opacity / 256);
							float progress = 1 - ((float)elapsed_time.count() / 500.0f) - pre_progressed;

							if (256 * progress < 1) guide_opacity = 1;
							else if ((int)(256 * progress) == 0) continue;
							else guide_opacity = (int)(256 * progress);
							SendMessage(hWnd, WM_PAINT, 0, 0);

							OutputDebugString(L"B");
						}
					}
					else Sleep(10);
				}
				OutputDebugString(L"C");
				return 0;
			}, (void *)hWnd, 0, (unsigned*)&dwThreadID);

			if (m_hDrawThread == 0) MessageBox(0, L"DrawThread Error\n", 0, 0);

			break;
		}
		case WM_GETMINMAXINFO:
		{
			LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
			lpMMI->ptMinTrackSize.x = 90;
			lpMMI->ptMinTrackSize.y = 90;
			break;
		}
		case WM_ERASEBKGND:
			return false;
		case WM_LBUTTONDOWN:
			bMinimizeClk = bCloseClk = false;

			if (Graphics::IsCursorInMinimize(hWnd))
				bMinimizeClk = true;
			else if (Graphics::IsCursorInClose(hWnd))
				bCloseClk = true;
			else 
				SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
			break;
		case WM_LBUTTONUP:
			if (bMinimizeClk && Graphics::IsCursorInMinimize(hWnd))
				ShowWindow(hWnd, SW_MINIMIZE);
			else if (bCloseClk && Graphics::IsCursorInClose(hWnd))
			{
				DestroyWindow(hWnd);
				PostQuitMessage(0);
			}
			bMinimizeClk = bCloseClk = false;
			break;
		case WM_CONTEXTMENU:
		{
			static HWND m_hWnd;
			m_hWnd = hWnd;

			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);

			HMENU hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDC_ANEMONE));
			hMenu = GetSubMenu(hMenu, 0);

			m_hCBTHook = SetWindowsHookEx(WH_CBT, [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT
			{
				if (nCode == HCBT_CREATEWND)
				{
					m_hMenuWnd = (HWND)wParam;
					WCHAR name[1024] = { 0 };
					GetClassName(m_hMenuWnd, name, sizeof(name));
					if (!wcscmp(name, L"#32768"))
					{
						int nExStyle_Menu = GetWindowLong(m_hMenuWnd, GWL_EXSTYLE);
						nExStyle_Menu |= WS_EX_NOACTIVATE;
						SetWindowLong(m_hMenuWnd, GWL_EXSTYLE, nExStyle_Menu);
						SetParent(m_hMenuWnd, m_hWnd);
					}
				}
				return ::CallNextHookEx(m_hCBTHook, nCode, wParam, lParam);
			}, g_hInst, GetWindowThreadProcessId(hWnd, NULL));
			if (!m_hCBTHook)
				MessageBox(0, L"CBTHook Error", 0, 0);

			TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);

			UnhookWindowsHookEx(m_hCBTHook);
			break;
		}
		case WM_NCHITTEST:
		{
			mouse_evt = true;
			latest_evt = message;

			POINT pt;
			RECT rc;
			GetClientRect(hWnd, &rc);
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			ScreenToClient(hWnd, &pt);

			int BORDERWIDTH = 5;

			/*top-left, top and top-right*/
			if (pt.y < BORDERWIDTH)
				if (pt.x < BORDERWIDTH)
					return HTTOPLEFT;
				else if (pt.x > (rc.right - BORDERWIDTH))
					return HTTOPRIGHT;
				else
					return HTTOP;
			/*bottom-left, bottom and bottom-right */
			if (pt.y > (rc.bottom - BORDERWIDTH))
				if (pt.x < BORDERWIDTH)
					return HTBOTTOMLEFT;
				else if (pt.x>(rc.right - BORDERWIDTH))
					return HTBOTTOMRIGHT;
				else
					return HTBOTTOM;
			if (pt.x < BORDERWIDTH)
				return HTLEFT;
			if (pt.x > (rc.right - BORDERWIDTH))
				return HTRIGHT;

			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
		case WM_SHOWWINDOW:
			SendMessage(hWnd, WM_PAINT, 0, 0);
		break;
		case WM_COMMAND:
			return OnCommand(hWnd, message, wParam, lParam);
		case WM_PAINT:
			return OnPaint(hWnd);
		case WM_DESTROY:
			UnhookWindowsHookEx(m_hMouseHook);
			TerminateThread(m_hMHThread, 0);
			TerminateThread(m_hDrawThread, 0);
			m_hMouseHook = NULL;
			PostQuitMessage(0);
			break;
		case WM_NCMOUSEMOVE:
		case WM_MOUSEMOVE:
			latest_evt = message;
			mouse_evt = true;

			TRACKMOUSEEVENT mouseEvent;
			mouseEvent.cbSize = sizeof(mouseEvent);
			mouseEvent.dwFlags = TME_LEAVE;
			mouseEvent.hwndTrack = hWnd;
			TrackMouseEvent(&mouseEvent);
			return OnPaint(hWnd);
		case WM_MOUSELEAVE:
			if (latest_evt == WM_NCHITTEST && message == WM_MOUSELEAVE) return 0;
			latest_evt = message;
			mouse_evt = false;
			return OnPaint(hWnd);
		case WM_MOVING:
		case WM_SIZING:
		{
			RECT * rect = (RECT *)lParam;
			SetWindowPos(hWnd, NULL, rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top, 0);
			return OnSize(hWnd, rect->right - rect->left, rect->bottom - rect->top);
		}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}

	LRESULT CALLBACK OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDM_SETTING:
			HWND hSettingWnd;
			hSettingWnd = CreateDialog(g_hInst,
				MAKEINTRESOURCE(IDD_SETTING),
				hWnd,
				(DLGPROC)SettingDlg::DlgProc);
			ShowWindow(hSettingWnd, SW_SHOW);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}

	LRESULT CALLBACK OnPaint(HWND hWnd)
	{
		Graphics::PaintLoop(hWnd);
		return 0;
	}
	LRESULT CALLBACK OnSize(HWND hWnd, int width, int height)
	{
		Graphics::Resize(hWnd, width, height);
		Graphics::PaintLoop(hWnd);
		return 0;
	}
}