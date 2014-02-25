#pragma once

#include <Windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <vsstyle.h>

#include "main_window.hpp"

template <class Derived>
class ListBoxProcessor{
	HTHEME theme;
	HFONT font;
	HBRUSH brush;

	void OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT *pdi)
	{
		if(pdi->CtlID == IDC_PICTURE_LIST){
			if(pdi->itemID == 0xffffffff)
				return;

			WCHAR buf[MAX_PATH];
			ListBox_GetText(pdi->hwndItem, pdi->itemID, buf);
			WCHAR *ptr = ::wcsrchr(buf, L'\\') + 1;

			HDC memdc;
			HPAINTBUFFER hpb = ::BeginBufferedPaint(pdi->hDC, &pdi->rcItem, BPBF_TOPDOWNDIB, nullptr, &memdc);

			::FillRect(memdc, &pdi->rcItem, brush);

			if(pdi->itemState & ODS_SELECTED){
				::BufferedPaintSetAlpha(hpb, &pdi->rcItem, 50);
			} else{
				::BufferedPaintSetAlpha(hpb, &pdi->rcItem, 0);
			}

			HGDIOBJ hold = ::SelectObject(memdc, font);

			RECT rc = pdi->rcItem;
			rc.right -= rc.left;
			rc.left = 0;
			rc.bottom -= rc.top;
			rc.top = 0;

			DTTOPTS o;
			o.dwSize = sizeof o;
			o.dwFlags = DTT_COMPOSITED | DTT_GLOWSIZE | DTT_TEXTCOLOR;
			o.crText = RGB(0, 0, 0);
			o.iGlowSize = 10;

			HRESULT hr = ::DrawThemeTextEx(
				theme, memdc, TEXT_BODYTITLE, 0,
				ptr, std::wcslen(ptr), DT_SINGLELINE | DT_NOPREFIX | DT_LEFT,
				const_cast<LPRECT>(&pdi->rcItem), &o);

			::SelectObject(memdc, hold);

			::EndBufferedPaint(hpb, TRUE);

//			::DrawTextW(pdi->hDC, ptr, ::wcslen(ptr), (LPRECT)&pdi->rcItem, DT_NOPREFIX | DT_NOCLIP);
		}
	}

	void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
	{
		if(id == IDC_PICTURE_LIST){
			int sel = ListBox_GetCurSel(hwndCtl);
			if(sel != -1)
				static_cast<Derived*>(this)->select_file(sel);
		}
	}

public:
	ListBoxProcessor()
	{
		NONCLIENTMETRICS ncm;
		ncm.cbSize = sizeof(ncm);
		::SystemParametersInfoW(
			SPI_GETNONCLIENTMETRICS,
			sizeof(ncm),
			&ncm,
			0
			);
		auto lf = ncm.lfCaptionFont;
		font = ::CreateFontIndirectW(&lf);

		brush = ::CreateSolidBrush(0xff000000 | RGB(0, 0, 0));
	}
	~ListBoxProcessor()
	{
		::DeleteObject(font);
		::DeleteObject(brush);
	}
	bool WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &lresult)
	{
		auto dummy = [&]() -> LRESULT{
			switch(msg){
			case WM_CREATE:
				theme = ::OpenThemeData(hwnd, VSCLASS_WINDOW);
				return 0;
			case WM_DESTROY:
				::CloseThemeData(theme);
				return 0;

				HANDLE_MSG(hwnd, WM_DRAWITEM, OnDrawItem);
				HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
			}
			return lresult;
		};

		lresult = dummy();
		return 1;
	}
};
