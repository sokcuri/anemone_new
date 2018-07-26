#include "stdafx.h"
#include "Anemone.h"
#include "SettingDlg.h"
#include "ScrollDialog.h"
#include "resource.h"

namespace SettingDlg
{
	static int lastSection = 0;

	INT_PTR CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		static HWND hDlg[10] = { 0, };
		switch (message)
		{
		case WM_INITDIALOG:
		{
			RECT rect;
			GetWindowRect(hWnd, &rect);

			int width = rect.right - rect.left;
			int height = rect.bottom - rect.top;

			int cx = GetSystemMetrics(SM_CXSCREEN);
			int cy = GetSystemMetrics(SM_CYSCREEN);

			MoveWindow(hWnd, (cx - width) / 2, (cy - height) / 2, width, height, false);

			InitSection(hWnd, hDlg);
			//SetForegroundWindow(hWnd);
			break;
		}
		case WM_SIZE:
			//ScrollDialog::OnSize(hWnd, HIWORD(wParam), LOWORD(wParam), lParam);
			break;

		case WM_HSCROLL:
			//ScrollDialog::OnHVScroll(hWnd, SB_HORZ, LOWORD(wParam));
			break;

		case WM_VSCROLL:
			//ScrollDialog::OnHVScroll(hWnd, SB_VERT, LOWORD(wParam));
			break;
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDOK:
				DestroyWindow(hWnd);
				break;
			case IDC_SECTION_GENERAL:
			case IDC_SECTION_TEXT:
			case IDC_SECTION_TRANSLATE:
			case IDC_SECTION_ETC:
				SwitchSection(hDlg, LOWORD(wParam) - 1000);
				break;
			}
		}
		break;
		case WM_CLOSE:
		case WM_QUIT:
			DestroyWindow(hWnd);
			break;
		}
		return 0;
	}

	void InitSection(HWND hWnd, HWND *hDlg)
	{
		for (int i = 0; i < 10; i++)
		{
			*(hDlg + i) = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_SETTING_GENERAL + i), hWnd, (DLGPROC)SectionProc);
			MoveWindow(hDlg[i], 130, 10, 520, 340, false);
		}
		ShowWindow(hDlg[lastSection], true);
	}

	void SwitchSection(HWND *hDlg, int id)
	{
		for (int i = 0; i < 10; i++)
			if (id != i)
				ShowWindow(hDlg[i], false);

		ShowWindow(hDlg[id], true);
		lastSection = id;
	}

	LRESULT CALLBACK myNewWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_MOUSEMOVE:
			SetCursor(LoadCursor(NULL, IDC_HAND));
			break;

		case WM_LBUTTONDOWN:
			//msg_dev("Button down!");
			break;
		}
		return false;
		//return CallWindowProc(hwnd, hwnd, uMsg, wParam, lParam);
	}

	BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam)
	{
		HWND hDlg = (HWND)lParam;
		wchar_t str[255];

		switch (GetDlgCtrlID(hWnd))
		{
		case IDC_CFG_TEXT_SPEED:
		{
			SendMessage(hWnd, TBM_SETRANGEMAX, 1, 100);
			SendMessage(hWnd, TBM_SETRANGEMIN, 1, 0);
			SendMessage(hWnd, TBM_SETPOS, 1, text_speed);
			break;
		}
		case IDC_CFG_GROUP_TEXT_SPEED:
		{
			wchar_t a[255], b[255];
			LoadStringW(g_hInst, GetDlgCtrlID(hWnd) + 10000, a, 255);
			swprintf_s(b, L"%s (%d)", a, text_speed);
			SetDlgItemText(hDlg, GetDlgCtrlID(hWnd), b);

			break;
		}
		default:
		{
			CheckDlgButton(hDlg, GetDlgCtrlID(hWnd), true);
			LoadStringW(g_hInst, GetDlgCtrlID(hWnd) + 10000, str, 255);
			if (str[0])
				SetWindowText(hWnd, str);
			break;
		}
		}
		return true;
	}
	INT_PTR CALLBACK SectionProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
	{
		UNREFERENCED_PARAMETER(lParam);
		switch (message)
		{
		case WM_INITDIALOG:
		{
			EnumChildWindows(hDlg, EnumChildProc, (LPARAM)hDlg);
			return (INT_PTR)TRUE;
		}
		case WM_HSCROLL:
			if (GetDlgItem(hDlg, IDC_CFG_TEXT_SPEED) == (HWND)lParam)
			{
				switch (LOWORD(wParam))
				{
				case TB_LINEUP:
				case TB_LINEDOWN:
				case TB_PAGEUP:
				case TB_PAGEDOWN:
				case TB_TOP:
				case TB_BOTTOM:
				case TB_ENDTRACK:
				case TB_THUMBTRACK:
				{
					if (LOWORD(wParam) == TB_THUMBTRACK)
					{
						text_speed = (INT)HIWORD(wParam);
					}
					else
					{
						text_speed = (INT)SendDlgItemMessage(hDlg, IDC_CFG_TEXT_SPEED, TBM_GETPOS, 0, 0);
					}

					wchar_t a[255], b[255];
					LoadStringW(g_hInst, IDC_CFG_GROUP_TEXT_SPEED + 10000, a, 255);
					swprintf_s(b, L"%s (%d)", a, text_speed);
					SetDlgItemText(hDlg, IDC_CFG_GROUP_TEXT_SPEED, b);
					break;
				}
				}
				OutputDebugString(L"IDC_CFG_TEXT_SPEED\n");
			}/*
			{
			case IDC_CFG_TEXT_SPEED:
			{
				switch (LOWORD(wParam))
				{
				case TB_THUMBPOSITION:
					OutputDebugString(L"TB_THUMBPOSITION\n");
					break;
				case TB_THUMBTRACK:
					OutputDebugString(L"TB_THUMBTRACK\n");
					break;
				}
				break;
			}
			}
			break;
			*/
			//ScrollDialog::OnHVScroll(hDlg, SB_HORZ, LOWORD(wParam));
			break;

		case WM_VSCROLL:
			//ScrollDialog::OnHVScroll(hDlg, SB_VERT, LOWORD(wParam));
			break;
		case WM_MOUSEWHEEL:

			break;
		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			break;
		}
		return (INT_PTR)FALSE;
	}
}