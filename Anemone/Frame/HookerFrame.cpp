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

	ITH_Init();
	ITH_CleanUp();
	/*DWORD dwThreadID;
	hHookThread = (HANDLE)_beginthreadex(NULL, 0, [](void* pData) -> unsigned int {
		::ITH_Init();
		::ITH_CleanUp();
		return 0;
	}, (void *)this, 0, (unsigned*)&dwThreadID);
	*/
}

HookerFrame::~HookerFrame()
{
}

bool HookerFrame::OnFirstProc()
{
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
	case IDM_CAPTION_BROWSE:
		BrowseFile();
		break;
	case IDM_CAPTION_GOTO:
	{
		bPauseKBHook = true;
		size_t goto_num = DialogBoxParam(nullptr, MAKEINTRESOURCE(IDD_CAPTION_GOTO), handle, [](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) -> INT_PTR {
			switch (message)
			{
			case WM_INITDIALOG:
			{
				auto p = reinterpret_cast<BaseFrame *>(lParam);
				int line = (p->n_selLine == -1 ? 1 : p->n_selLine + 1);
				HWND hwndOwner = GetParent(hWnd);
				RECT rc, rcDlg, rcOwner;

				GetWindowRect(hwndOwner, &rcOwner);
				GetWindowRect(hWnd, &rcDlg);
				CopyRect(&rc, &rcOwner);

				// Offset the owner and dialog box rectangles so that right and bottom 
				// values represent the width and height, and then offset the owner again 
				// to discard space taken up by the dialog box. 

				OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
				OffsetRect(&rc, -rc.left, -rc.top);
				OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

				// The new position is the sum of half the remaining space and the owner's 
				// original position. 

				SetWindowPos(hWnd,
					HWND_TOP,
					rcOwner.left + (rc.right / 2),
					rcOwner.top + (rc.bottom / 2),
					0, 0,          // Ignores size arguments. 
					SWP_NOSIZE);
				SetForegroundWindow(hWnd);

				if (GetDlgCtrlID((HWND)wParam) != IDC_CAPTION_GOTO_EDIT)
				{
					SetFocus(GetDlgItem(hWnd, IDC_CAPTION_GOTO_EDIT));
					return FALSE;
				}
				SetDlgItemText(hWnd, IDC_CAPTION_GOTO_EDIT, std::to_wstring(line).c_str());

				std::wstring s;
				s = L"ÁÙ ¹øÈ£";
				s += L"(1 - ";
				s += std::to_wstring((p->vecBuff.size() ? p->vecBuff.size() : 1));
				s += L"): ";
				SetDlgItemText(hWnd, IDC_CAPTION_GOTO_STATIC, s.c_str());
				return true;
			}
			case WM_COMMAND:
			{
				int wmId = LOWORD(wParam);
				switch (wmId)
				{
				case IDOK:
				{
					int n = -1;
					WCHAR str[255];
					GetDlgItemText(hWnd, IDC_CAPTION_GOTO_EDIT, str, 255);
					try
					{
						n = std::stoi(str);
					}
					catch (std::exception e) {}
					return EndDialog(hWnd, n);
					break;
				}
				case IDCANCEL:
					return EndDialog(hWnd, 0);
				}
				break;
			}
			case WM_CLOSE:
				EndDialog(hWnd, 0);
				break;
			}
			return 0;
		}, reinterpret_cast<LPARAM>(this));
		bPauseKBHook = false;

		if (vecBuff.size() <= goto_num - 1) break;
		n_selLine = goto_num - 1;

		bBypass = false;
		strText = vecBuff[n_selLine];
		typeNum = 0;
		break;
	}
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

