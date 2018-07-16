#include "stdafx.h"
#include "Graphics.h"
#include "LayeredWnd.h"

namespace Graphics
{
	int w;
	int h;

	DisplayParams fDisplayParams;
	SkAutoMalloc fSurfaceMemory;
	sk_sp<SkSurface> fBackbufferSurface;

	auto GetGraphicsFromContext()
	{
		return fBackbufferSurface->getCanvas();
	}

	void Init()
	{
		SkGraphics::Init();
	}

	void Resize(HWND hWnd, int fWidth, int fHeight)
	{
		return CreateContext(hWnd, fWidth, fHeight);
	}

	void CreateContext(HWND hWnd, int fWidth, int fHeight)
	{
		w = fWidth;
		h = fHeight;

		if (fWidth <= 1) fWidth = 1;
		if (fHeight <= 1) fHeight = 1;


		fBackbufferSurface.reset();
		const size_t bmpSize = sizeof(BITMAPINFOHEADER) + (fWidth * fHeight * sizeof(uint32_t));
		fSurfaceMemory.reset(bmpSize);

		BITMAPINFO *bmpInfo = reinterpret_cast<BITMAPINFO *>(fSurfaceMemory.get());
		ZeroMemory(bmpInfo, sizeof(BITMAPINFO));

		bmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmpInfo->bmiHeader.biWidth = fWidth;
		bmpInfo->bmiHeader.biHeight = -fHeight; // 스키아는 톱다운 형식으로 그리기 때문에 음수를 넣어 톱다운 비트맵 생성
		bmpInfo->bmiHeader.biPlanes = 1;
		bmpInfo->bmiHeader.biBitCount = 32;
		bmpInfo->bmiHeader.biCompression = BI_RGB;

		void *pixels = bmpInfo->bmiColors;

		SkImageInfo info = SkImageInfo::Make(fWidth, fHeight, fDisplayParams.fColorType, kPremul_SkAlphaType, fDisplayParams.fColorSpace);
		fBackbufferSurface = SkSurface::MakeRasterDirect(info, pixels, sizeof(uint32_t) * fWidth);
	}

	void SwapBuffer(HWND hWnd, int fWidth, int fHeight)
	{
		BITMAPINFO *bmpInfo = reinterpret_cast<BITMAPINFO*>(fSurfaceMemory.get());
		HDC dc = GetDC(hWnd);
		StretchDIBits(dc, 0, 0, fWidth, fHeight, 0, 0, fWidth, fHeight, bmpInfo->bmiColors, bmpInfo, DIB_RGB_COLORS, SRCCOPY);
		ReleaseDC(hWnd, dc);
	}

	void DrawSysMenu(HWND hWnd, SkBitmap &bitmap)
	{
		RECT rect;
		GetWindowRect(hWnd, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		POINT p;
		bool bHoverMin = false;
		bool bHoverClose = false;

		// Transparent clear
		SkRect rt = { (SkScalar)width, (SkScalar)height };
		SkCanvas frameCanvas(bitmap);
		frameCanvas.clear(SK_ColorTRANSPARENT);

		// Drawing outer background
		SkPaint paint;
		SkRRect outer, inner;
		paint.setColor(SkColorSetRGB(250, 250, 250));
		paint.setStyle(SkPaint::kFill_Style);
		outer = SkRRect::MakeRect({ 0, 0, width + 0.0f, height + 0.0f });
		inner = SkRRect::MakeRect({ 5.0f, 30.0f, width - 5.0f, height - 30.0f });
		frameCanvas.drawDRRect(outer, inner, paint);

		// Drawing innerframe border
		paint.reset();
		paint.setAntiAlias(false);
		paint.setColor(SkColorSetRGB(200, 200, 200));
		paint.setStyle(SkPaint::kStroke_Style);
		paint.setStrokeWidth(1);
		frameCanvas.drawRect({ 5.0f, 30.0f, width - 5.0f, height - 30.0f }, paint);

		// Draw to Title
		paint.reset();
		paint.setAntiAlias(true);
		paint.setColor(SkColorSetRGB(0, 0, 0));
		paint.setTextSize(14.0f);
		frameCanvas.drawText("Anemone v2.0", 14, 10.0f, 20.0f, paint);

		// Draw to Sysmenu button padding
		paint.reset();
		paint.setAntiAlias(true);
		paint.setStyle(SkPaint::kFill_Style);
		paint.setColor(SkColorSetRGB(250, 250, 250));
		frameCanvas.drawRect({ width - 80.0f, 0.0f, width + 0.0f, 30.0f }, paint);

		paint.reset();
		paint.setAntiAlias(true);
		if (GetCursorPos(&p))
		{
			ScreenToClient(hWnd, &p);
			if (IsCursorInMinimize(hWnd)) // Minimize button
			{
				paint.setStyle(SkPaint::kFill_Style);
				paint.setColor(SkColorSetRGB(229, 230, 231));
				frameCanvas.drawRect(GetMinimizeRect(hWnd), paint);
				bHoverMin = true;
			}
			else if (IsCursorInClose(hWnd)) // Close button
			{
				paint.setStyle(SkPaint::kFill_Style);
				paint.setColor(SK_ColorRED);
				frameCanvas.drawRect(GetCloseRect(hWnd), paint);
				bHoverClose = true;
			}
		}

		// Draw minimize icon
		paint.reset();
		paint.setAntiAlias(false);
		paint.setStrokeWidth(1.0f);
		paint.setColor(SkColorSetRGB(0, 0, 0));
		frameCanvas.drawLine(width - 54.0f, 15.0f, width - 42.0f, 15.0f, paint);

		// Draw close icon
		paint.reset();
		paint.setAntiAlias(true);
		paint.setStrokeWidth(1.01f);
		if (bHoverClose)
			paint.setColor(SK_ColorWHITE);
		frameCanvas.drawLine(width - 21.0f, 10.0f, width - 11.0f, 20.0f, paint);
		frameCanvas.drawLine(width - 11.0f, 10.0f, width - 21.0f, 20.0f, paint);

		// Drawing frame border
		paint.reset();
		paint.setAntiAlias(true);
		paint.setColor(SkColorSetRGB(200, 200, 200));
		paint.setStyle(SkPaint::kStroke_Style);
		paint.setStrokeWidth(1);
		frameCanvas.drawRect(rt, paint);
	}

	void PaintLoop(HWND hWnd)
	{
		RECT rect;
		GetWindowRect(hWnd, &rect);
		int cx = GetSystemMetrics(SM_CXSCREEN);
		int cy = GetSystemMetrics(SM_CYSCREEN);

		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		PAINTSTRUCT ps;
		HDC hDC, memDC;

		hDC = BeginPaint(hWnd, &ps);
		memDC = CreateCompatibleDC(hDC);
		auto *canvas = GetGraphicsFromContext();
		canvas->clear(SK_ColorTRANSPARENT);

/* -- guide
		SkPaint paint_point;
		paint_point.setAntiAlias(true);
		paint_point.setColor(SkColorSetARGB(guide_opacity, 200, 200, 200));
		paint_point.setStyle(SkPaint::kStroke_Style);
		paint_point.setStrokeWidth(10);

		canvas->drawRect(rt, paint_point);
*/

		// Create typeface
		sk_sp<SkFontMgr> mgr(SkFontMgr::RefDefault());
		sk_sp<SkTypeface> tf(
			mgr->matchFamilyStyleCharacter(
				nullptr, SkFontStyle(), nullptr, 0, L'가'));

		// Draw system menu (title-bar)
		SkPaint paint;
		SkBitmap bitmap;
		bitmap.allocPixels(SkImageInfo::MakeN32Premul(width, height));
		DrawSysMenu(hWnd, bitmap);
		paint.setAlpha(guide_opacity);
		canvas->drawBitmap(bitmap, 0, 0, &paint);

		paint.reset();
		canvas->save();
		canvas->clipRect({ 5.0f, 30.0f, width - 5.0f, height - 40.0f });
		
		SkPaint paint_o2, paint_o1, paint_shad, paint_fill;
		SkString name;
		SkRect bounds;
		SkScalar textWidth;
		std::wstring measuring_texts;

		int fWidth = rect.right - rect.left;
		int fHeight = rect.bottom - rect.top;

		float fPadding, fInnerWidth, fInnerHeight, fLineBreak;
		wchar_t szStr[] = L"소레 요리 NO";

		measuring_texts.clear();

		paint.setTextSize(36.0f);
		paint.setAntiAlias(true);
		paint.setTextEncoding(SkPaint::kUTF16_TextEncoding);
		paint.setTypeface(tf);
		paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
		paint.setStrokeJoin(SkPaint::Join::kRound_Join);

		paint_o2 = paint_o1 = paint_fill = paint_shad = paint;

		paint_shad.setColor(SkColorSetARGB(128, 160, 160, 160));
		paint_shad.setStyle(SkPaint::kStrokeAndFill_Style);
		paint_shad.setStrokeWidth(14.0f);
		paint_shad.setAlpha(127);

		paint_shad.setMaskFilter(SkMaskFilter::MakeBlur(
			kNormal_SkBlurStyle,
			SkBlurMask::ConvertRadiusToSigma(SkIntToScalar(6))));

		paint_o2.setColor(SkColorSetARGB(255, 214, 255, 251));
		paint_o2.setStyle(SkPaint::kStroke_Style);
		paint_o2.setStrokeWidth(14.0f);

		paint_o1.setColor(SkColorSetRGB(46, 196, 182));
		paint_o1.setStyle(SkPaint::kStrokeAndFill_Style);
		paint_o1.setStrokeWidth(8.0f);

		paint_fill.setColor(SkColorSetRGB(255, 255, 255));
		paint_fill.setStyle(SkPaint::kFill_Style);
		paint_fill.setStrokeWidth(0.0f);

		std::vector<SkPaint> paints{ paint_shad, paint_o2, paint_o1, paint_fill };

		for (auto &p : paints)
		{
			measuring_texts.clear();
			if (p == paint_shad)
				fPadding = 15.0f;
			else
				fPadding = 10.0f;

			fLineBreak = 18.0f;
			fInnerWidth = fPadding + 10.0f;
			fInnerHeight = fPadding + paint_o2.getTextSize() + 35.0f;

			for (size_t i = 0; i <= wcslen(szStr); i++)
			{
				if (!measuring_texts.length() && szStr[i] == L' ') continue;
				measuring_texts += szStr[i];
				textWidth = paint_o2.measureText(measuring_texts.c_str(), measuring_texts.length() * 2, &bounds);
				if (textWidth + fLineBreak * 2 > fWidth || i == wcslen(szStr) || szStr[i] == '\n')
				{
					measuring_texts = measuring_texts.substr(0, measuring_texts.length() - 1);

					canvas->drawText(measuring_texts.c_str(), measuring_texts.length() * 2, fInnerWidth, fInnerHeight, p);
					fInnerHeight += paint_o2.getTextSize() + 5.0f;

					if (szStr[i] == '\n' || szStr[i] == ' ')
						measuring_texts.clear();
					else
						measuring_texts = szStr[i];
				}
			}
		}
		canvas->restore();
		canvas->flush();

		BYTE * pBits;
		HBITMAP hBitmap;
		BITMAPINFOHEADER bmih;
		bmih.biSize = sizeof(BITMAPINFOHEADER);
		bmih.biWidth = w;
		bmih.biHeight = h;
		bmih.biPlanes = 1;
		bmih.biBitCount = 32;
		bmih.biCompression = BI_RGB;
		bmih.biSizeImage = 0;
		bmih.biXPelsPerMeter = 0;
		bmih.biYPelsPerMeter = 0;
		bmih.biClrUsed = 0;
		bmih.biClrImportant = 0;

		hBitmap = CreateDIBSection(NULL, (BITMAPINFO *)&bmih, 0, (VOID**)&pBits, NULL, 0);
		SelectObject(memDC, hBitmap);

		BITMAPINFO *bmpInfo = reinterpret_cast<BITMAPINFO*>(fSurfaceMemory.get());
		StretchDIBits(memDC, 0, 0, w, h, 0, 0, w, h, bmpInfo->bmiColors, bmpInfo, DIB_RGB_COLORS, SRCCOPY);

		POINT dcOffset = { 0, 0 };
		GetWindowRect(hWnd, &rect);
		SIZE size = { rect.right - rect.left, rect.bottom - rect.top };

		BLENDFUNCTION bf;
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = 255;
		bf.AlphaFormat = AC_SRC_ALPHA;

		UpdateLayeredWindow(hWnd, hDC, 0, &size, memDC, &dcOffset, 0, &bf, ULW_ALPHA);

		EndPaint(hWnd, &ps);

		ReleaseDC(NULL, hDC);
		ReleaseDC(NULL, memDC);

		DeleteObject(hBitmap);
		DeleteObject(memDC);
		DeleteObject(hDC);
	}

	bool IsCursorInMinimize(HWND hWnd)
	{
		RECT rect;
		GetWindowRect(hWnd, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		POINT p;
		if (GetCursorPos(&p))
		{
			ScreenToClient(hWnd, &p);
			return (p.x >= width - 64.0f && p.x <= width - 32.0f &&
				p.y >= 0.0f && p.y <= 30.0f);
		}
		return false;
	}
	bool IsCursorInClose(HWND hWnd)
	{
		RECT rect;
		GetWindowRect(hWnd, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		POINT p;
		if (GetCursorPos(&p))
		{
			ScreenToClient(hWnd, &p);
			return (p.x >= width - 32.0f && p.x <= width &&
				p.y >= 0.0f && p.y <= 30.0f);
		}
		return false;
	}
	SkRect GetMinimizeRect(HWND hWnd)
	{
		RECT rect;
		GetWindowRect(hWnd, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		return { width - 64.0f, 0.0f, width - 32.0f, 30.0f };
	}
	SkRect GetCloseRect(HWND hWnd)
	{
		RECT rect;
		GetWindowRect(hWnd, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		return { width - 32.0f, 0.0f, width - 0.0f, 30.0f };
	}
}