#include "stdafx.h"
#include "LayeredWindow.h"

LayeredWindow::LayeredWindow()
{
	Initialize();
}

LayeredWindow::~LayeredWindow()
{
	CloseWindow(handle);
}


void LayeredWindow::Initialize()
{
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);

	int width = 650;
	int height = 200;

	int x = (cx - width) / 2;
	int y = (cy - height) / 2;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)this->WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = nullptr;
	wcex.hIcon = /*LoadIcon(nullptr, MAKEINTRESOURCE(IDI_ANEMONE));*/ nullptr;
	wcex.hCursor = /*LoadCursor(nullptr, IDC_ARROW);*/ nullptr;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;
	wcex.hIconSm = /*LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));*/ nullptr;
}

inline LRESULT LayeredWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_CREATE:
	{
	}
	case WM_GETMINMAXINFO:
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMinTrackSize.x = 90;
		lpMMI->ptMinTrackSize.y = 90;
		break;
	}
	case WM_ERASEBKGND:
		return false;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_CONTEXTMENU:
	case WM_NCHITTEST:
	case WM_SHOWWINDOW:
		SendMessage(hWnd, WM_PAINT, 0, 0);
		break;
	case WM_COMMAND:
	case WM_PAINT:
	case WM_DESTROY:
	case WM_NCMOUSEMOVE:
	case WM_MOUSEMOVE:
	case WM_MOUSELEAVE:
	case WM_MOVING:
	case WM_SIZING:
	case WM_ACTIVATE:
	{
		//mouse_evt = true;
	}
	//break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void LayeredWindow::Resize(int width, int height)
{
	buffer->Resize(width, height);
	delete context;
	context = new LayeredContext(handle, buffer);
}

void LayeredWindow::Create(bool bShow = true)
{
	std::wstring temp_class_name = std::to_wstring(GetTickCount());
	wcex.lpszClassName = temp_class_name.c_str();
	RegisterClassExW(&wcex);

	handle = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_APPWINDOW, temp_class_name.c_str(), title.c_str(), WS_POPUP,
		position.x, position.y, size.cx, size.cy, nullptr, nullptr, nullptr, nullptr);
	
	if (!handle)
	{
		MessageBox(0, L"창 생성에 실패했습니다", 0, MB_ICONERROR);
		return;
	}

	if (bShow)
		ShowWindow(handle, true);


	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);

	int width = 300;
	int height = 500;
	

	int x = (cx - width) / 2;
	int y = (cy - height) / 2;

	DWORD dwThreadID;


	buffer = new LayeredBuffer(width, height, kBGRA_8888_SkColorType, kPremul_SkAlphaType);
	context = new LayeredContext(handle, buffer);

	context->SetPosition(x, y);
	HANDLE thread = (HANDLE)_beginthreadex(NULL, 0, [](void* pData) -> unsigned int {
		auto *p = (LayeredWindow *)pData;
		// p->Resize(width, height);

		while (1)
		{
			p->fps.Update();
			p->OnRender();
			p->context->Present();
			Sleep(1);
		}
		return 0;
	}, (void *)this, 0, (unsigned*)&dwThreadID);

	if (thread == 0)
	{
		MessageBox(0, L"Thread Create Error\n", 0, 0);
		return;
	}

	HACCEL hAccelTable = /*LoadAccelerators(nullptr, MAKEINTRESOURCE(IDC_ANEMONE));*/ nullptr;
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

void LayeredWindow::OnRender()
{
	auto canvas_size = buffer->GetSize();
	int width = canvas_size->cx;
	int height = canvas_size->cy;

	SkPaint paint;
	int r = rand() % 255;
	int g = rand() % 255;
	int b = rand() % 255;
	context->clear(SkColorSetRGB(255, 255, 255));

	paint.reset();
	paint.setAntiAlias(true);
	//paint.setTextEncoding(SkPaint::kUTF16_TextEncoding);
	paint.setColor(SkColorSetRGB(0, 0, 0));
	paint.setTextSize(18.0f);
	paint.setTextAlign(SkPaint::Align::kRight_Align);
	std::string z;
	z = std::to_string(fps.lastFps);
	z += "fps";

	paint.setStyle(SkPaint::kStroke_Style);
	paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
	paint.setStrokeJoin(SkPaint::Join::kRound_Join);
	paint.setStrokeWidth(3.0f);
	paint.setColor(SkColorSetARGB(128, 255, 255, 255));
	context->drawText(z.c_str(), z.length(), width - 10.0f, height - paint.getTextSize() - 26.0f, paint);

	paint.setStyle(SkPaint::kFill_Style);
	paint.setColor(SkColorSetRGB(0, 0, 0));
	context->drawText(z.c_str(), z.length(), width - 10.0f, height - paint.getTextSize() - 26.0f, paint);

}
