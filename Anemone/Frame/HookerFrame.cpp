#include "stdafx.h"
#include "HookerFrame.h"
#include "../../third_party/ithvnr/include/gui/main.h"

HookerFrame::HookerFrame()
{
	bBypass = false;
	bPauseKBHook = false;
	n_selLine = -1;

	m_mode = HOOKER_MODE;
	m_title = L"(Hooker Mode)";
	typeNum = 0;

	DWORD dwThreadID;
	hHookThread = (HANDLE)_beginthreadex(NULL, 0, [](void* pData) -> unsigned int {
		::ITH_Init();
		::ITH_CleanUp();
		return 0;
	}, (void *)this, 0, (unsigned*)&dwThreadID);
	
}

HookerFrame::~HookerFrame()
{
	::ITH_CleanUp();
}

INT_PTR CALLBACK HookerFrame::HookerStickProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

	case WM_LBUTTONDOWN:
		SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		break;
	case WM_MOVING:
	case WM_SIZING:
	{
		RECT * rect = (RECT *)lParam;
		int x = rect->left;
		int y = rect->top;
		int w = rect->right - rect->left;
		int h = rect->bottom - rect->top;
		SetWindowPos(hWnd, nullptr, x, y, w, h, 0);

		return 0;
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
		case ID_HOOKERMODE:
		case ID_SETTING:
			EndDialog(hWnd, wmId);
			break;
		}
		break;
	}
	case WM_CLOSE:
		EndDialog(hWnd, 0x12345678);
		break;
	}
	return 0;
}

bool HookerFrame::OnFirstProc(HWND hWnd)
{
	//CreateDialog(nullptr, MAKEINTRESOURCE(IDD_HOOKER_STICK), nullptr, (DLGPROC)HookerFrame::HookerStickProc);
	BrowseFile();
	return true;
}

bool HookerFrame::BrowseFile()
{
	return true;
}

void HookerFrame::DrawLineCount()
{
	if (n_selLine == -1) return;

	SkPaint paint;
	paint.setAntiAlias(true);
	//paint.setTextEncoding(SkPaint::kUTF16_TextEncoding);
	paint.setColor(SkColorSetRGB(0, 0, 0));
	paint.setTextSize(18.0f);
	paint.setTextAlign(SkPaint::Align::kRight_Align);
	std::string z;
	z += std::to_string(n_selLine + 1);
	z += " / ";
	z += std::to_string(vecBuff.size());

	paint.setStyle(SkPaint::kStroke_Style);
	paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
	paint.setStrokeJoin(SkPaint::Join::kRound_Join);
	paint.setStrokeWidth(3.0f);
	paint.setColor(SkColorSetARGB(128, 255, 255, 255));

	context->drawText(z.c_str(), z.length(), size.cx - 10.0f, size.cy - paint.getTextSize() - 46.0f, paint);

	paint.setStyle(SkPaint::kFill_Style);
	paint.setColor(SkColorSetRGB(0, 0, 0));
	context->drawText(z.c_str(), z.length(), size.cx - 10.0f, size.cy - paint.getTextSize() - 46.0f, paint);
}

bool HookerFrame::OnRender()
{
	context->clear(SK_ColorTRANSPARENT);

	if (!text_speed || bBypass)
	{
		strBuff = strText;
		typeNum = strBuff.length() + 1;
	}
	else
	{
		if (!typeNum)
		{
			lastTime = system_clock::now();
			typeNum++;
		}

		if (typeNum <= strText.length())
		{
			milli_duration duration = system_clock::now() - lastTime;
			if (duration.count() > text_speed)
			{
				strBuff = strText.substr(0, typeNum++);
				lastTime = system_clock::now();
			}
		}
	}

	LayeredWindow::OnRender();
	DrawSysMenu();
	DrawLineCount();

	DrawContent();
	return true;
}

bool HookerFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int wmId = LOWORD(wParam);
	switch (wmId)
	{
	case IDM_HOOKER_PROCESS:
		ITH_OpenDialog(ITH_PROCESS_DLG);
		break;
	case IDM_HOOKER_OPTION:
		ITH_OpenDialog(ITH_OPTION_DLG);
		break;
	default:
		return BaseFrame::OnCommand(wParam, lParam);
	}
	return true;
}

bool HookerFrame::OnKeyboardHookProc(WPARAM wParam, LPARAM lParam)
{
	PKBDLLHOOKSTRUCT pHookKey = (PKBDLLHOOKSTRUCT)lParam;
	if (bPauseKBHook) return false;

	switch (wParam)
	{
	case 256:	// WM_KEYDOWN
	case 260:	// WM_SYSKEYDOWN

		if (pHookKey->vkCode == VK_PRIOR)
		{
			if (!n_selLine || n_selLine == -1) break;
			bBypass = false;
			strText = vecBuff[--n_selLine];
			typeNum = 0;
			return true;
		}
		else if (pHookKey->vkCode == VK_NEXT)
		{
			if (n_selLine >= 0 && typeNum != strText.length() + 1)
			{
				bBypass = true;
				typeNum = strText.length() + 1;
				strText = vecBuff[n_selLine];
				return true;
			}

			bBypass = false;
			if (n_selLine + 1 == vecBuff.size()) break;
			strText = vecBuff[++n_selLine];
			typeNum = 0;
			return true;
		}
		break;
	}
	return false;
}

