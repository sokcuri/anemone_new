#include "stdafx.h"
#include "CaptionFrame.h"
#include "compact_enc_det/compact_enc_det.h"


CaptionFrame::CaptionFrame()
{
	bBypass = false;
	bPauseKBHook = false;
	n_selLine = -1;

	m_mode = CAPTION_MODE;
	m_title = L"(Caption Mode)";
	typeNum = 0;
}

CaptionFrame::~CaptionFrame()
{
}

bool CaptionFrame::OnFirstProc(HWND hWnd)
{
	BrowseFile();
	return true;
}

bool CaptionFrame::BrowseFile()
{
	OPENFILENAME ofn;
	wchar_t szFile[260];

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = handle;
	ofn.lpstrFile = szFile;

	ofn.lpstrFile[0] = L'\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"텍스트 파일\0*.TXT\0모든 파일\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn) == TRUE)
	{
		FILE *fp;
		if (_wfopen_s(&fp, ofn.lpstrFile, L"rb") != 0)
		{
			MessageBox(0, L"파일을 열 수 없음", 0, 0);
			return false;
		}
		fseek(fp, 0, SEEK_END);
		long fsize = ftell(fp);
		fseek(fp, 0, SEEK_SET);  //same as rewind(f);

		wchar_t *wtext = 0;
		char *text = (char *)malloc(fsize + 1);
		char *c = text;
		fread(text, fsize, 1, fp);
		fclose(fp);

		text[fsize] = 0;

		bool is_reliable;
		int bytes_consumed;

		Encoding encoding = CompactEncDet::DetectEncoding(
			text, strlen(text),
			nullptr, nullptr, nullptr,
			UNKNOWN_ENCODING,
			Language::KOREAN,
			CompactEncDet::QUERY_CORPUS,
			false,
			&bytes_consumed,
			&is_reliable);

		if (fsize <= 2)
		{
			if (c[0] == 0xFE && c[1] == 0xFF)
				c += 2;
			else if (c[0] == 0xFF && c[1] == 0xFE)
				c += 2;
		}
		else
		{
			if (memcmp(c, "\xFF\xFE\x00\x00", 4))
				c += 4;
			else if (memcmp(c, "\x00\x00\xFE\xFF", 4))
				c += 4;
			else if (memcmp(c, "\xFE\xFF", 2))
				c += 2;
			else if (memcmp(c, "\xFF\xFE", 2))
				c += 2;
		}

		int SourceEncoding = CP_ACP;
		if (encoding == Encoding::UTF8)
			SourceEncoding = CP_UTF8;
		else if (encoding == Encoding::KOREAN_EUC_KR)
			SourceEncoding = 949;
		else if (encoding == Encoding::JAPANESE_CP932 || Encoding::JAPANESE_SHIFT_JIS || Encoding::JAPANESE_EUC_JP || Encoding::JAPANESE_JIS)
			SourceEncoding = 932;
		else if (encoding == Encoding::KDDI_ISO_2022_JP || encoding == Encoding::SOFTBANK_ISO_2022_JP)
			SourceEncoding = 50222;
		else if (encoding == Encoding::ISO_2022_KR)
			SourceEncoding = 50225;
		int nLen = MultiByteToWideChar(SourceEncoding, 0, c, strlen(c), NULL, NULL);
		wtext = (wchar_t *)malloc((nLen + 1) * 2);
		MultiByteToWideChar(SourceEncoding, 0, c, strlen(c), wtext, nLen);
		wtext[nLen] = 0;

		std::wstringstream wss(wtext);
		free(text);
		free(wtext);

		std::wstring s;
		vecBuff.clear();

		while (std::getline(wss, s))
		{
			s.erase(0, s.find_first_not_of(' '));
			s.erase(s.find_last_not_of(' ') + 1);
			if (s[0] == L'\n' || s[0] == L'\r' && s[1] == L'\n' || s[0] == L'\r' && s[1] == 0) continue;
			if (!s.length()) continue;
			vecBuff.push_back(s);
		}

		std::wstring filename = ofn.lpstrFile;
		filename = filename.substr(filename.rfind('\\') + 1);
		if (vecBuff.size())
		{
			strText = L"파일을 읽었습니다. \n> " + filename;
			n_selLine = -1;
			typeNum = 0;
			bBypass = true;
		}
	}
	return true;
}

void CaptionFrame::DrawLineCount()
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

bool CaptionFrame::OnRender()
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

bool CaptionFrame::OnCommand(WPARAM wParam, LPARAM lParam)
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
				s = L"줄 번호";
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

bool CaptionFrame::OnKeyboardHookProc(WPARAM wParam, LPARAM lParam)
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
	return true;
}

