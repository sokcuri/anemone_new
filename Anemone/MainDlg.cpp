#include "stdafx.h"
#include "MainDlg.h"
#include "Anemone.h"
#include "CaptionFrame.h"
#include "ClipboardFrame.h"
#include "TranslateFrame.h"
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

			SetWindowText(hWnd, g_AnemoneInfo);
			break;
		}
		case WM_SHOWWINDOW:
			break;
		case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			switch (wmId)
			{
			case ID_CAPTIONMODE:
			case ID_CLIPMODE:
			case ID_TRANSMODE:
			case ID_SETTING:
				EndDialog(hWnd, wmId);
				break;
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