/**
* This file is part of Anemone.
*
* Anemone is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* The Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Anemone is distributed in the hope that it will be useful,
*
* But WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Anemone.
*
*   If not, see <http://www.gnu.org/licenses/>.
*
**/

#include "stdafx.h"
#include "LayeredWindow.h"
#include "BaseFrame.h"

LayeredWindow *LayeredWindow::lastWindow = nullptr;

LayeredWindow::LayeredWindow()
{
	InitializeCriticalSection(&cs);
	Initialize();
}

LayeredWindow::~LayeredWindow()
{
	if (lastWindow == this) lastWindow = nullptr;
	delete this->buffer;
	delete this->context;
	DeleteCriticalSection(&cs);
	CloseWindow(handle);
}

void LayeredWindow::Initialize()
{
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)this->_WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = nullptr;
	wcex.hIcon = LoadIcon(nullptr, MAKEINTRESOURCE(IDI_ANEMONE));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));;
}

bool LayeredWindow::OnCreate()
{
	return true;
}

LRESULT LayeredWindow::_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	auto p = reinterpret_cast<LayeredWindow *>(GetWindowLongPtr(hWnd, GWL_USERDATA));
	
	switch (message)
	{
	case WM_NCCREATE:
	{
		auto p = reinterpret_cast<LayeredWindow *>((reinterpret_cast<CREATESTRUCT *>(lParam))->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(p));

		if (!p->handle) p->handle = hWnd;
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	}
	return p->WndProc(hWnd, message, wParam, lParam);
}

bool LayeredWindow::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool LayeredWindow::OnDestroy()
{
	TerminateThread(thread, 0);
	PostQuitMessage(0);
	return true;
}

bool LayeredWindow::OnChangeCbChain(WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool LayeredWindow::OnDrawClipboard()
{
	return false;
}

bool LayeredWindow::OnDestroyClipboard()
{
	return false;
}

bool LayeredWindow::OnKeyboardHookProc(WPARAM wParam, LPARAM lParam)
{
	return false;
}

LRESULT LayeredWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_USER + 100:
		return OnFirstProc();
	case WM_USER + 200:
		return OnKeyboardHookProc(wParam, lParam);
	case WM_CREATE:
	{
		PostMessage(hWnd, WM_USER + 100, 0, 0);
		return OnCreate();
	}
	case WM_GETMINMAXINFO:
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMinTrackSize.x = 90;
		lpMMI->ptMinTrackSize.y = 90;
		break;
	}
	case WM_CHANGECBCHAIN:
		return OnChangeCbChain(wParam, lParam);
	case WM_DRAWCLIPBOARD:
		return OnDrawClipboard();
	case WM_DESTROYCLIPBOARD:
		return OnDestroyClipboard();
	case WM_ERASEBKGND:
	case WM_PAINT:
		return false;
	case WM_COMMAND:
		return OnCommand(wParam, lParam);
	case WM_DESTROY:
		return OnDestroy();
	case WM_NCMOUSEMOVE:
	case WM_MOUSEMOVE:
		mouse_evt = true;

		TRACKMOUSEEVENT mouseEvent;
		mouseEvent.cbSize = sizeof(mouseEvent);
		mouseEvent.dwFlags = TME_LEAVE;
		mouseEvent.hwndTrack = hWnd;
		TrackMouseEvent(&mouseEvent);
		return 0;
	case WM_MOUSELEAVE:
		//if (latest_evt == WM_NCHITTEST && message == WM_MOUSELEAVE) return 0;
		//latest_evt = message;
		mouse_evt = false;
		return 0;

	case WM_LBUTTONDOWN:
		SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		break;

	case WM_NCHITTEST:
	{
		RECT rc;
		GetClientRect(hWnd, &rc);

		RawPoint pt(LOWORD(lParam), HIWORD(lParam));
		ScreenToClient(hWnd, &pt);

		int BORDERWIDTH = 5;

		/* top-left, top and top-right */
		if (pt.y < BORDERWIDTH)
			if (pt.x < BORDERWIDTH)
				return HTTOPLEFT;
			else if (pt.x >(rc.right - BORDERWIDTH))
				return HTTOPRIGHT;
			else
				return HTTOP;
		/* bottom-left, bottom and bottom-right */
		if (pt.y >(rc.bottom - BORDERWIDTH))
			if (pt.x < BORDERWIDTH)
				return HTBOTTOMLEFT;
			else if (pt.x>(rc.right - BORDERWIDTH))
				return HTBOTTOMRIGHT;
			else
				return HTBOTTOM;
		if (pt.x < BORDERWIDTH)
			return HTLEFT;
		if (pt.x >(rc.right - BORDERWIDTH))
			return HTRIGHT;
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
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

		EnterCriticalSection(&cs);

		Resize(w, h);
		context->SetPosition(x, y);

		LeaveCriticalSection(&cs);
		return 0;
	}
	case WM_CONTEXTMENU:
	{
		static HWND m_hWnd;
		m_hWnd = hWnd;

		RawPoint pt(LOWORD(lParam), HIWORD(lParam));
		HMENU hMenu = GetSubMenu(LoadMenu(NULL, MAKEINTRESOURCE(IDC_CLIPBOARD - 1 + m_mode)), 0);
		HHOOK hCBTHook = SetWindowsHookEx(WH_CBT, [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			if (nCode == HCBT_CREATEWND)
			{
				HWND m_hMenuWnd = (HWND)wParam;
				WCHAR name[1024] = { 0 };
				GetClassName(m_hMenuWnd, name, sizeof(name));

				if (!wcscmp(name, L"#32768"))
				{
					int nExStyle_Menu = GetWindowLong(m_hMenuWnd, GWL_EXSTYLE);
					SetWindowLong(m_hMenuWnd, GWL_EXSTYLE, nExStyle_Menu |= WS_EX_NOACTIVATE);
					SetParent(m_hMenuWnd, m_hWnd);
					SetWindowText(m_hMenuWnd, L"AnemoneContextMenu");
				}
			}
			return ::CallNextHookEx(0, nCode, wParam, lParam);
		}, nullptr, GetWindowThreadProcessId(hWnd, NULL));
		if (!hCBTHook)
			MessageBox(0, L"CBTHook Error", 0, 0);

		TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
		UnhookWindowsHookEx(hCBTHook);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT LayeredWindow::KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	return ::CallNextHookEx(nullptr, nCode, wParam, lParam);
}

void LayeredWindow::Resize(int width, int height)
{
	size.cx = width;
	size.cy = height;

	buffer->Resize(width, height);
	delete context;
	context = new LayeredContext(handle, buffer);
}

bool LayeredWindow::Create()
{
	if (lastWindow) return false;

	std::wstring temp_class_name = std::to_wstring(GetTickCount());
	wcex.lpszClassName = temp_class_name.c_str();
	RegisterClassExW(&wcex);

	handle = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_APPWINDOW | WS_EX_TOPMOST, temp_class_name.c_str(), title.c_str(), WS_POPUP,
		position.x, position.y, size.cx, size.cy, nullptr, nullptr, nullptr, this);
	
	if (!handle)
	{
		MessageBox(0, L"창 생성에 실패했습니다", 0, MB_ICONERROR);
		return false; 
	}

	ShowWindow(handle, true);


	int width = 800;
	int height = 200;

	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);
	int x = (cx - width) / 2;
	int y = (cy - height) / 2;

	DWORD dwThreadID;

	size.cx = width;
	size.cy = height;
	buffer = new LayeredBuffer(width, height, kBGRA_8888_SkColorType, kPremul_SkAlphaType);
	context = new LayeredContext(handle, buffer);
	context->SetPosition(x, y);

	thread = (HANDLE)_beginthreadex(NULL, 0, [](void* pData) -> unsigned int {
		while (lastWindow)
		{
			lastWindow->fps.Update();

			EnterCriticalSection(&lastWindow->cs);
			lastWindow->OnRender();
			LeaveCriticalSection(&lastWindow->cs);

			lastWindow->context->Present();
			Sleep(1);
		}
		return 0;


	}, (void *)this, 0, (unsigned*)&dwThreadID);

	if (thread == 0)
	{
		MessageBox(0, L"Thread Create Error\n", 0, 0);
		return false;
	}

	HACCEL hAccelTable = nullptr;
	MSG msg;
	lastWindow = this;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return true;
}

bool LayeredWindow::OnRender()
{
	static int nStep = 0;
	nStep++;

	auto canvas_size = buffer->GetSize();
	int width = canvas_size->cx;
	int height = canvas_size->cy;

	SkPaint paint;
	context->clear(SkColorSetARGB(128, 255, 255, 255));

	paint.reset();
	paint.setAntiAlias(true);
	paint.setColor(SkColorSetRGB(0, 0, 0));
	paint.setTextSize(18.0f);
	paint.setTextAlign(SkPaint::Align::kRight_Align);
	std::string z = fps.ToString();

	paint.setStyle(SkPaint::kStroke_Style);
	paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
	paint.setStrokeJoin(SkPaint::Join::kRound_Join);
	paint.setStrokeWidth(3.0f);
	paint.setColor(SkColorSetARGB(128, 255, 255, 255));
	context->drawText(z.c_str(), z.length(), width - 10.0f, height - paint.getTextSize() - 26.0f, paint);

	paint.setStyle(SkPaint::kFill_Style);
	paint.setColor(SkColorSetRGB(0, 0, 0));
	context->drawText(z.c_str(), z.length(), width - 10.0f, height - paint.getTextSize() - 26.0f, paint);

	return true;
}

bool LayeredWindow::OnFirstProc()
{
	return false;
}