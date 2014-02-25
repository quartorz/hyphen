#pragma once

#include <Windows.h>
#include <windowsx.h>

#include "main_window.hpp"

inline void ListBox_OnPaint(HWND hwnd)
{
	RECT rc;
	::GetClientRect(hwnd, &rc);

	PAINTSTRUCT ps;
	HDC hdc = ::BeginPaint(hwnd, &ps);

	HDC memdc;
	HPAINTBUFFER pb = ::BeginBufferedPaint(hdc, &rc, BPBF_TOPDOWNDIB, nullptr, &memdc);

	if(pb != nullptr){
		::BufferedPaintSetAlpha(pb, &rc, 0);
		HBRUSH white = static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH));
		::FillRect(hdc, &rc, white);
		::DeleteObject(white);
		::SendMessageW(hwnd, WM_PRINTCLIENT, reinterpret_cast<WPARAM>(memdc), PRF_CLIENT);
		::EndBufferedPaint(pb, TRUE);
	}

	::EndPaint(hwnd, &ps);
}

inline void ListBox_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	if(vk != VK_LEFT && vk != VK_UP && vk != VK_RIGHT && vk != VK_DOWN)
		return FORWARD_WM_KEYDOWN(hwnd, vk, cRepeat, flags, ::DefSubclassProc);
}

inline LRESULT CALLBACK ListBox_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR nIdSubclass, DWORD_PTR dwRefData)
{
	auto w = reinterpret_cast<main_window*>(dwRefData);

	switch(msg){
		HANDLE_MSG(hwnd, WM_PAINT, ListBox_OnPaint);
		HANDLE_MSG(hwnd, WM_KEYDOWN, ListBox_OnKey);

	case WM_ERASEBKGND:
		return 0;
	}

	return ::DefSubclassProc(hwnd, msg, wParam, lParam);
}
