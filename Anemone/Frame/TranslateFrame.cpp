#include "stdafx.h"
#include "TranslateFrame.h"


TranslateFrame::TranslateFrame()
{
	mode = TRANSLATE_MODE;
	advTitle = L"(Translate Mode)";
}


TranslateFrame::~TranslateFrame()
{
}

bool TranslateFrame::OnRender()
{
	context->clear(SK_ColorTRANSPARENT);
	wchar_t str[255];
	swprintf_s(str, L"time: %lld", GetTickCount64());
	strBuff = str;

	LayeredWindow::OnRender();
	DrawSysMenu();
	DrawContent();
	return true;
}
