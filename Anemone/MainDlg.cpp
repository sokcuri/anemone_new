#include "stdafx.h"
#include "MainDlg.h"
#include "Anemone.h"
#include "SettingDlg.h"

namespace MainDlg
{
	INT_PTR CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
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

			std::wstring params = L" -- ";
			params += WIDEN(TXT(GIT_CUR_COMMIT));
			params += L" - ";
			params += WIDEN(__TIMESTAMP__);
			std::wstring title = WIDEN(ANEMONE_VER) + params;
			SetWindowText(hWnd, title.c_str());
			break;
		}
		case WM_SHOWWINDOW:
			break;
		case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			switch (wmId)
			{
			case ID_CLIPMODE:
			case ID_CAPTIONMODE:
			case ID_TRANSMODE:
			case ID_SETTING:
				return EndDialog(hWnd, wmId);
			}
			break;
		}
		case WM_CLOSE:
			EndDialog(hWnd, 0);
			break;
		}
		return 0;
	}
}