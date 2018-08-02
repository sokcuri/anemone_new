#include "stdafx.h"
#include "ClipboardFrame.h"

ClipboardFrame::ClipboardFrame()
{
	m_mode = CLIPBOARD_MODE;
	m_title = L"(Clipboard Mode)";

	bBypass = false;

	bFirst = true;
	bStart = false;
	typeNum = 0;

	InitializeCriticalSection(&cs);
}

ClipboardFrame::~ClipboardFrame()
{
	DeleteCriticalSection(&cs);
}

bool ClipboardFrame::OnRender()
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
	DrawContent();
	return true;
}

bool ClipboardFrame::OnCreate()
{
	BaseFrame::OnCreate();

	HWND hwndNextViewer = SetClipboardViewer(handle);
	return true;
}

bool ClipboardFrame::OnChangeCbChain(WPARAM wParam, LPARAM lParam)
{
	if ((HWND)wParam == hwndNextViewer)
		hwndNextViewer = (HWND)lParam;

	else if (hwndNextViewer != NULL)
		SendMessage(hwndNextViewer, WM_CHANGECBCHAIN, wParam, lParam);

	return true;
}

bool ClipboardFrame::OnDrawClipboard()
{
	EnterCriticalSection(&cs);

	bool success = false;
	int nRetryCount = 0;

	while (nRetryCount < 5)
	{
		if (OpenClipboard(handle))
		{
			success = true;
			break;
		}
		nRetryCount++;
		Sleep(10);
	}

	if (!success)
	{
		auto text = fmt::format(L"cannot open clipboard : %d\n", GetLastError());
		OutputDebugString(text.c_str());
		LeaveCriticalSection(&cs);
		return false;
	}

	if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
	{
		CloseClipboard();
		LeaveCriticalSection(&cs);
		return false;
	}

	auto hglb = GetClipboardData(CF_UNICODETEXT);
	if (!hglb) return false;
	auto pchData = reinterpret_cast<wchar_t *>(GlobalLock(hglb));
	if (!pchData) return false;
	if (bFirst)
	{
		bFirst = false;
	}
	else
	{
		bStart = true;
		strText = pchData;
	}
	typeNum = 0;
	GlobalUnlock(hglb);
	CloseClipboard();

	LeaveCriticalSection(&cs);
	return true;
}

bool ClipboardFrame::OnDestroy()
{
	ChangeClipboardChain(handle, hwndNextViewer);
	return BaseFrame::OnDestroy();
}
