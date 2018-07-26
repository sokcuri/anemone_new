#include "stdafx.h"
#include "ClipboardFrame.h"


ClipboardFrame::ClipboardFrame()
{
	m_mode = CLIPBOARD_MODE;
	advTitle = L"(Clipboard Mode)";
}


ClipboardFrame::~ClipboardFrame()
{
}

bool ClipboardFrame::OnRender()
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
