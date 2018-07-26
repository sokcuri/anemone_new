#include "stdafx.h"
#include "CaptionFrame.h"


CaptionFrame::CaptionFrame()
{
	typeNum = 0;
	bBypass = false;
	m_mode = CAPTION_MODE;
	n_selLine = -1;
	advTitle = L"(Caption Mode)";
	strText = L"캡션 모드입니다";
}


CaptionFrame::~CaptionFrame()
{
}

bool CaptionFrame::OnFirstProc()
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
		if (_wfopen_s(&fp, ofn.lpstrFile, L"rt,ccs=UTF-8") != 0)
		{
			MessageBox(0, L"파일을 열 수 없음", 0, 0);
			return false;
		}
		std::wstring filename = ofn.lpstrFile;

		WCHAR buff[2048];
		std::wstring s;

		vecBuff.clear();

		while (fgetws(buff, 2048, fp))
		{
			s = buff;
			s.erase(0, s.find_first_not_of(' '));
			s.erase(s.find_last_not_of(' ') + 1);
			if (s[0] == '\n' || s[0] == '\r' && s[1] == '\n') continue;
			if (!s.length()) continue;
			vecBuff.push_back(s);
		}

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

	if (bBypass)
	{
		strBuff = strText;
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
			OutputDebugString(std::to_wstring(duration.count()).c_str());
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
	default:
		return BaseFrame::OnCommand(wParam, lParam);
	}
	return true;
}

LRESULT CaptionFrame::KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0)
	{
		PKBDLLHOOKSTRUCT pHookKey = (PKBDLLHOOKSTRUCT)lParam;

		switch (wParam)
		{
		case 256:	// WM_KEYDOWN
		case 260:	// WM_SYSKEYDOWN

			if (pHookKey->vkCode == VK_NUMPAD9)
			{
				if (!n_selLine) break;
				bBypass = false;
				strText = vecBuff[--n_selLine];
				typeNum = 0;
				return true;
			}
			else if (pHookKey->vkCode == VK_NUMPAD3)
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
	}
	return ::CallNextHookEx(nullptr, nCode, wParam, lParam);
}

