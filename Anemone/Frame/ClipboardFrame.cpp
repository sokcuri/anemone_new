#include "stdafx.h"
#include "ClipboardFrame.h"

ClipboardFrame::ClipboardFrame()
{
	bBypass = false;

	bFirst = true;
	bStart = false;

	m_mode = CLIPBOARD_MODE;
	advTitle = L"(Clipboard Mode)";
	typeNum = 0;
}

ClipboardFrame::~ClipboardFrame()
{
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
		wchar_t str[255];
		swprintf_s(str, L"cannot open clipboard: %d\n", GetLastError());
		OutputDebugString(str);
		return false;
	}

	if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
	{
		CloseClipboard();
		return false;
	}

	auto hglb = GetClipboardData(CF_UNICODETEXT);
	auto pchData = reinterpret_cast<wchar_t *>(GlobalLock(hglb));
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
	return true;
}

bool ClipboardFrame::OnDestroy()
{
	ChangeClipboardChain(handle, hwndNextViewer);
	return BaseFrame::OnDestroy();
}
