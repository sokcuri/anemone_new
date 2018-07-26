#include "stdafx.h"
#include "BaseFrame.h"


BaseFrame::BaseFrame()
{
}


BaseFrame::~BaseFrame()
{
}

RawSize BaseFrame::GetWindowSize()
{
	RECT rect;
	GetWindowRect(handle, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	return RawSize(width, height);
}

void BaseFrame::DrawSysMenu()
{
	RawSize size = GetWindowSize();

	POINT p;
	bool bHoverMin = false;
	bool bHoverClose = false;

	SkRect rt = { (SkScalar)size.cx, (SkScalar)size.cy };

	// Drawing outer background
	SkPaint paint;
	SkRRect outer, inner;
	paint.setColor(SkColorSetRGB(250, 250, 250));
	paint.setStyle(SkPaint::kFill_Style);
	outer = SkRRect::MakeRect({ 0, 0, size.cx + 0.0f, size.cy + 0.0f });
	inner = SkRRect::MakeRect({ 5.0f, 30.0f, size.cx - 5.0f, size.cy - 30.0f });
	context->drawDRRect(outer, inner, paint);

	// Drawing innerframe border
	paint.reset();
	paint.setAntiAlias(false);
	paint.setColor(SkColorSetRGB(200, 200, 200));
	paint.setStyle(SkPaint::kStroke_Style);
	paint.setStrokeWidth(1);
	context->drawRect({ 5.0f, 30.0f, size.cx - 5.0f, size.cy - 30.0f }, paint);

	// Draw to Title
	paint.reset();
	paint.setAntiAlias(true);
	paint.setColor(SkColorSetRGB(0, 0, 0));
	paint.setTextEncoding(SkPaint::kUTF16_TextEncoding);
	paint.setTextSize(14.0f);
	std::wstring title = L"Anemone v2.0 " + advTitle;
	context->drawText(title.c_str(), title.length() * 2, 10.0f, 20.0f, paint);

	// Draw to Sysmenu button padding
	paint.reset();
	paint.setAntiAlias(true);
	paint.setStyle(SkPaint::kFill_Style);
	paint.setColor(SkColorSetRGB(250, 250, 250));
	context->drawRect({ size.cx - 80.0f, 0.0f, size.cx + 0.0f, 30.0f }, paint);

	paint.reset();
	paint.setAntiAlias(true);
	if (GetCursorPos(&p))
	{
		ScreenToClient(handle, &p);
		/*
		if (IsCursorInMinimize(hWnd)) // Minimize button
		{
			paint.setStyle(SkPaint::kFill_Style);
			paint.setColor(SkColorSetRGB(229, 230, 231));
			context->drawRect(GetMinimizeRect(hWnd), paint);
			bHoverMin = true;
		}
		else if (IsCursorInClose(hWnd)) // Close button
		{
			paint.setStyle(SkPaint::kFill_Style);
			paint.setColor(SK_ColorRED);
			context->drawRect(GetCloseRect(hWnd), paint);
			bHoverClose = true;
		}
		*/
	}

	// Draw minimize icon
	paint.reset();
	paint.setAntiAlias(false);
	paint.setStrokeWidth(1.0f);
	paint.setColor(SkColorSetRGB(0, 0, 0));
	context->drawLine(size.cx - 54.0f, 15.0f, size.cx - 42.0f, 15.0f, paint);

	// Draw close icon
	paint.reset();
	paint.setAntiAlias(true);
	paint.setStrokeWidth(1.01f);
	if (bHoverClose)
		paint.setColor(SK_ColorWHITE);
	context->drawLine(size.cx - 21.0f, 10.0f, size.cx - 11.0f, 20.0f, paint);
	context->drawLine(size.cx - 11.0f, 10.0f, size.cx - 21.0f, 20.0f, paint);

	// Drawing frame border
	paint.reset();
	paint.setAntiAlias(true);
	paint.setColor(SkColorSetRGB(200, 200, 200));
	paint.setStyle(SkPaint::kStroke_Style);
	paint.setStrokeWidth(1);
	context->drawRect(rt, paint);
}

void BaseFrame::DrawContent()
{
	RawSize size = GetWindowSize();

	SkPaint paint;
	context->save();
	context->clipRect({ 5.0f, 30.0f, size.cx - 5.0f, size.cy - 40.0f });

	SkPaint paint_o2, paint_o1, paint_shad, paint_fill;
	SkString name;
	SkRect bounds;
	SkScalar textWidth;
	std::wstring measuring_texts;

	// Create typeface
	sk_sp<SkFontMgr> mgr(SkFontMgr::RefDefault());
	sk_sp<SkTypeface> tf(
		mgr->matchFamilyStyleCharacter(
			nullptr, SkFontStyle(), nullptr, 0, L'°¡'));

	float fPadding, fInnerWidth, fInnerHeight, fLineBreak;

	measuring_texts.clear();

	paint.reset();
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

		std::wstring szStr = strBuff;
		for (size_t i = 0; i <= szStr.length(); i++)
		{
			if (!measuring_texts.length() && szStr[i] == L' ') continue;
			measuring_texts += szStr[i];
			textWidth = paint_o2.measureText(measuring_texts.c_str(), measuring_texts.length() * 2, &bounds);
			if (textWidth + fLineBreak * 2 > size.cx || i == szStr.length() || szStr[i] == '\n')
			{
				measuring_texts = measuring_texts.substr(0, measuring_texts.length() - 1);

				context->drawText(measuring_texts.c_str(), measuring_texts.length() * 2, fInnerWidth, fInnerHeight, p);
				fInnerHeight += paint_o2.getTextSize() + 5.0f;

				if (szStr[i] == '\n' || szStr[i] == ' ')
					measuring_texts.clear();
				else
					measuring_texts = szStr[i];
			}
		}
	}

	context->restore();
	context->flush();
}

bool BaseFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int wmId = LOWORD(wParam);
	switch (wmId)
	{
	case IDM_SETTING:
		HWND hSettingWnd;
		hSettingWnd = CreateDialog(nullptr, MAKEINTRESOURCE(IDD_SETTING), handle, (DLGPROC)SettingDlg::DlgProc);
		ShowWindow(hSettingWnd, SW_SHOW);
		break;
	case IDM_EXIT:
		DestroyWindow(handle);
		break;
	default:
		return false;
	}
	return true;
}
/*
LRESULT AnemoneFrame::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//OutputDebugString(L"AnemoneFrame::WndProc\n");
	switch (message)
	{
	case WM_COMMAND:
		if (OnCommand(wParam, lParam))
			return true;
	}

	return LayeredWindow::WndProc(hWnd, message, wParam, lParam);
}
*/