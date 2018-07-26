#include "stdafx.h"
#include "CaptionFrame.h"


CaptionFrame::CaptionFrame()
{
	mode = CAPTION_MODE;
	advTitle = L"(Caption Mode)";
	strText = L"캡션 모드입니다";
}


CaptionFrame::~CaptionFrame()
{
}

bool CaptionFrame::OnFirstProc()
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
		std::wstring str;
		while (fgetws(buff, 2048, fp))
		{
			str = buff;
			str.erase(0, str.find_first_not_of(' '));
			str.erase(str.find_last_not_of(' ') + 1);
			if (!str.length()) continue;
			vecBuff.push_back(str);
		}

		if (vecBuff.size())
		{
			strText = L"파일을 읽었습니다 >> " + filename;
			n_selLine = -1;
		}
	}
	return true;
}

bool CaptionFrame::OnRender()
{
	context->clear(SK_ColorTRANSPARENT);
	strBuff = strText;

	BaseFrame::OnRender();
	DrawSysMenu();
	DrawContent();
	return true;
}