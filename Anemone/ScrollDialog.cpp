#include "stdafx.h"
#include "ScrollDialog.h"

namespace ScrollDialog
{
	void OnHVScroll(HWND handle, int bar, UINT code)
	{
		const int scrollPos = GetScrollPos(handle, bar, code);

		if (scrollPos == -1)
			return;

		SetScrollPos(handle, bar, scrollPos, TRUE);
		ScrollClient(handle, bar, scrollPos);
	}

	void ScrollClient(HWND handle, int bar, int pos)
	{
		static int s_prevx = 1;
		static int s_prevy = 1;

		int cx = 0;
		int cy = 0;

		int& delta = (bar == SB_HORZ ? cx : cy);
		int& prev = (bar == SB_HORZ ? s_prevx : s_prevy);

		delta = prev - pos;
		prev = pos;

		if (cx || cy)
		{
			ScrollWindow(handle, cx, cy, NULL, NULL);
		}
	}

	int GetScrollPos(HWND handle, int bar, UINT code)
	{
		SCROLLINFO si = {};
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
		GetScrollInfo(handle, bar, &si);

		const int minPos = si.nMin;
		const int maxPos = si.nMax - (si.nPage - 1);

		int result = -1;

		switch (code)
		{
		case SB_LINEUP /*SB_LINELEFT*/:
			result = std::max(si.nPos - 1, minPos);
			break;

		case SB_LINEDOWN /*SB_LINERIGHT*/:
			result = std::min(si.nPos + 1, maxPos);
			break;

		case SB_PAGEUP /*SB_PAGELEFT*/:
			result = std::max(si.nPos - (int)si.nPage, minPos);
			break;

		case SB_PAGEDOWN /*SB_PAGERIGHT*/:
			result = std::min(si.nPos + (int)si.nPage, maxPos);
			break;

		case SB_TOP /*SB_LEFT*/:
			result = minPos;
			break;

		case SB_BOTTOM /*SB_RIGHT*/:
			result = maxPos;
			break;

		case SB_THUMBTRACK:
			result = si.nTrackPos;
			break;

		case SB_THUMBPOSITION:
		case SB_ENDSCROLL:
			break;
		}
		return result;
	}

	void OnInit(HWND hWnd)
	{
		RECT rc = {};
		GetClientRect(hWnd, &rc);

		const SIZE sz = { rc.right - rc.left, rc.bottom - rc.top };

		SCROLLINFO si = {};
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
		si.nPos = si.nMin = 1;

		si.nMax = sz.cx;
		si.nPage = sz.cx;
		SetScrollInfo(hWnd, SB_HORZ, &si, FALSE);

		si.nMax = sz.cy;
		si.nPage = sz.cy;
		SetScrollInfo(hWnd, SB_VERT, &si, FALSE);
		return;
	}

	bool OnSize(HWND hWnd, int cx, int cy, int state)
	{
		if (state != SIZE_RESTORED && state != SIZE_MAXIMIZED)
			return false;

		SCROLLINFO si = {};
		si.cbSize = sizeof(SCROLLINFO);

		const int bar[] = { SB_HORZ, SB_VERT };
		const int page[] = { cx, cy };

		for (size_t i = 0; i < ARRAYSIZE(bar); ++i)
		{
			si.fMask = SIF_PAGE;
			si.nPage = page[i];
			SetScrollInfo(hWnd, bar[i], &si, TRUE);

			si.fMask = SIF_RANGE | SIF_POS;
			GetScrollInfo(hWnd, bar[i], &si);

			const int maxScrollPos = si.nMax - (page[i] - 1);

			// Scroll client only if scroll bar is visible and window's
			// content is fully scrolled toward right and/or bottom side.
			// Also, update window's content on maximize.
			const bool needToScroll =
				(si.nPos != si.nMin && si.nPos == maxScrollPos) ||
				(state == SIZE_MAXIMIZED);

			if (needToScroll)
				ScrollClient(hWnd, bar[i], si.nPos);
		}
		return true;
	}
}