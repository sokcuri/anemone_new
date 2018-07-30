#include "stdafx.h"
#include "TranslateFrame.h"

TranslateFrame::TranslateFrame()
{
	m_mode = TRANSLATE_MODE;
	m_title = L"(Translate Mode)";
}

TranslateFrame::~TranslateFrame()
{
}

bool TranslateFrame::OnRender()
{
	context->clear(SK_ColorTRANSPARENT);
	strBuff = fmt::format(L"time: {0}", GetTickCount64());

	LayeredWindow::OnRender();
	DrawSysMenu();
	DrawContent();
	return true;
}
