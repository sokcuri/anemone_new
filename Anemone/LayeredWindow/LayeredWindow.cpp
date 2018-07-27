#include "stdafx.h"
#include "LayeredWindow.h"
#include "BaseFrame.h"

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

void LayeredWindow::StartEventCapture()
{
	RAWINPUTDEVICE rawInputDev[2];
	ZeroMemory(rawInputDev, sizeof(RAWINPUTDEVICE) * 2);

	// 키보드 RAWINPUTDEVICE 구조체 설정 
	rawInputDev[0].usUsagePage = 0x01;
	rawInputDev[0].usUsage = 0x06;
	rawInputDev[0].dwFlags = RIDEV_INPUTSINK;
	rawInputDev[0].hwndTarget = handle;

	//마우스 RAWINPUTDEVICE 구조체 설정
	rawInputDev[1].usUsagePage = 0x01;
	rawInputDev[1].usUsage = 0x02;
	rawInputDev[1].dwFlags = RIDEV_INPUTSINK;
	rawInputDev[1].hwndTarget = handle;

	if (FALSE == RegisterRawInputDevices(rawInputDev, 2, sizeof(RAWINPUTDEVICE)))
	{
		std::wstring str;
		OutputDebugString(L"RegisterRawInputDevices Error %d");
	}

}



void LayeredWindow::EndEventCapture()
{
	RAWINPUTDEVICE rawInputDev[2];
	ZeroMemory(rawInputDev, sizeof(RAWINPUTDEVICE) * 2);

	// 키보드 RAWINPUTDEVICE 구조체 설정 
	rawInputDev[0].usUsagePage = 0x01;
	rawInputDev[0].usUsage = 0x06;
	rawInputDev[0].dwFlags = RIDEV_REMOVE;
	rawInputDev[0].hwndTarget = handle;

	//마우스 RAWINPUTDEVICE 구조체 설정
	rawInputDev[1].usUsagePage = 0x01;
	rawInputDev[1].usUsage = 0x02;
	rawInputDev[1].dwFlags = RIDEV_REMOVE;
	rawInputDev[1].hwndTarget = handle;

	if (FALSE == RegisterRawInputDevices(rawInputDev, 2, sizeof(RAWINPUTDEVICE)))
	{
		std::wstring str;
		OutputDebugString(L"RegisterRawInputDevices Error %d");
	}

}
bool LayeredWindow::OnCreate()
{
	return true;
}

template<typename Base, typename T>
inline bool Instanceof(const T *ptr) {
	return dynamic_cast<const Base*>(ptr) != nullptr;
}

LRESULT LayeredWindow::_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	auto p = reinterpret_cast<LayeredWindow *>(GetWindowLongPtr(hWnd, GWL_USERDATA));
	
	switch (message)
	{
	case WM_NCCREATE:
	{
		auto p = reinterpret_cast<LayeredWindow *>(((CREATESTRUCT*)lParam)->lpCreateParams);
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
		OnFirstProc();
		break;
	case WM_USER + 200:
		return OnKeyboardHookProc(wParam, lParam);
	case WM_INPUT:
	{
		UINT dwSize = 40;
		static BYTE lpb[40];
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
		RAWINPUT* raw = (RAWINPUT*)lpb;

		if (raw->header.dwType == RIM_TYPEMOUSE)
		{
			int xPosRelative = raw->data.mouse.lLastX;
			int yPosRelative = raw->data.mouse.lLastY;
			if (raw->data.mouse.ulButtons == 0) // move
			{
				OutputDebugString(L"mouse move\n");
			}
			else
				if (raw->data.mouse.ulButtons == 1 /*|| m_bLbuttonDragging*/) // l-button down 
				{
					OutputDebugString(L"l-button down\n");
				}
			if (raw->data.mouse.ulButtons == 2) // l-button up
			{
				OutputDebugString(L"l-button up\n");
			}

			if (raw->data.mouse.ulButtons == 4) // r-button down
			{
				OutputDebugString(L"r-button down\n");
			}

			if (raw->data.mouse.ulButtons == 8) // r-button down
			{
				OutputDebugString(L"r-button up\n");
			}

		}

		if (raw->header.dwType == RIM_TYPEKEYBOARD)
		{
			WCHAR str[255];
			swprintf_s(str, L" Kbd: make=%04x Flags:%04x Reserved:%04x ExtraInformation:%08x, msg=%04x VK=%04x \n",
				raw->data.keyboard.MakeCode,
				raw->data.keyboard.Flags,
				raw->data.keyboard.Reserved,
				raw->data.keyboard.ExtraInformation,
				raw->data.keyboard.Message,
				raw->data.keyboard.VKey);

			MSG msg;
			memset(&msg, 0, sizeof(MSG));
			msg.hwnd = hWnd;
			msg.message = raw->data.keyboard.Message;
			msg.wParam = raw->data.keyboard.VKey;

			// KeyMessageProcessing( &msg );

			OutputDebugString(str);
		}
		break;
	}
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
		OutputDebugString(L"WM_CHANGECBCHAIN\n");
		return OnChangeCbChain(wParam, lParam);
	case WM_DRAWCLIPBOARD:
		OutputDebugString(L"WM_DRAWCLIPBOARD\n");
		return OnDrawClipboard();
	case WM_DESTROYCLIPBOARD:
		OutputDebugString(L"WM_DESTROYCLIPBOARD\n");
		return OnDestroyClipboard();
		break;
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
	if (nCode >= 0)
	{
		PKBDLLHOOKSTRUCT pHookKey = (PKBDLLHOOKSTRUCT)lParam;

		switch (wParam)
		{
		case 256:	// WM_KEYDOWN
		case 260:	// WM_SYSKEYDOWN
/*
			if (pHookKey->vkCode == VK_NUMPAD9)
			{
				if (m_mode == CAPTION_MODE)
				{
					if (!n_selLine) break;
					strBuff = vecBuff[--n_selLine];
					SendMessage(handle, WM_PAINT, 0, 0);
				}
			}
			else if (pHookKey->vkCode == VK_NUMPAD3)
			{
				if (m_mode == CAPTION_MODE)
				{
					if (n_selLine + 1 == vecBuff.size()) break;
					strBuff = vecBuff[++n_selLine];
					SendMessage(handle, WM_PAINT, 0, 0);
				}
			}*/
			break;
		}
	}
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

	HACCEL hAccelTable = /*LoadAccelerators(nullptr, MAKEINTRESOURCE(IDC_ANEMONE));*/ nullptr;
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
	//paint.setTextEncoding(SkPaint::kUTF16_TextEncoding);
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