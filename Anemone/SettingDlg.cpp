#include "stdafx.h"
#include "Anemone.h"
#include "SettingDlg.h"
#include "ScrollDialog.h"
#include "resource.h"

namespace SettingDlg
{
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
		ShowWindow(hDlg[0], true);
	}

	void SwitchSection(HWND *hDlg, int id)
	{
		for (int i = 0; i < 10; i++)
			if (id != i)
				ShowWindow(hDlg[i], false);

		ShowWindow(hDlg[id], true);
	}
	BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam)
	{
		HWND hDlg = (HWND)lParam;
		wchar_t str[255];
		GetWindowText(hWnd, str, 255);
		//if (!wcscmp(str, L"IDC_CFG_SIZE_GUIDE"))
		{
			//MessageBox(0, str, 0, 0);
			CheckDlgButton(hDlg, GetDlgCtrlID(hWnd), true);
			wchar_t szCtrlText[255];
			LoadStringW(g_hInst, GetDlgCtrlID(hWnd) + 10000, szCtrlText, 255);
			if (szCtrlText[0])
				SetWindowText(hWnd, szCtrlText);
			return true;
		}
		return false;
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