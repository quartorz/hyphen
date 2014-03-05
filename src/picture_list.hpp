#pragma once

#include "main_view.hpp"
#include "listbox.hpp"

#include <quote/win32/dialogs.hpp>
#include <quote/direct2d/flat_button.hpp>

#pragma comment(lib, "Comctl32")

class main_view_type::picture_list
	: public direct2d::userdefined_object<main_view_type::picture_list>
{
	using base = direct2d::userdefined_object<main_view_type::picture_list>;

	class button_base: public direct2d::flat_button{
		using base = direct2d::flat_button;

		std::function<void(void)> cb;

	public:
		button_base()
		{
			set_color(state::none, direct2d::color(0, 0, 0, 50));
			set_color(state::hover, direct2d::color(50, 50, 50, 20));
			set_color(state::push, direct2d::color(0, 0, 0, 80));
			set_text_color(state::none, direct2d::color(0, 0, 0, 200));
			set_text_color(state::hover, direct2d::color(0, 0, 0));
			set_text_color(state::push, direct2d::color(0, 0, 0, 150));
			set_text_size(30);
		}
		void set_callback(const std::function<void(void)> &f)
		{
			cb = f;
		}
		void on_push() override
		{
			cb();
		}
		void draw(const direct2d::paint_params &pp) override
		{
			pp.target->PushAxisAlignedClip(this->get_rect(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
			pp.target->Clear(direct2d::color(0, 0, 0, 0));
			pp.target->PopAxisAlignedClip();
			this->base::draw(pp);
		}
	};

	HWND list;
	HFONT font;
	button_base button;

	direct2d::point pos;

	std::uint32_t index = 1;

public:
	picture_list(main_window &w)
	{
		list = ::CreateWindowExW(
			0,
			WC_LISTBOXW,
			L"list",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_OWNERDRAWVARIABLE | LBS_HASSTRINGS | LBS_NOTIFY,
			0,
			0,
			0,
			0,
			w.get_hwnd(),
			reinterpret_cast<HMENU>(IDC_PICTURE_LIST),
			::GetModuleHandleW(nullptr),
			nullptr);
		::SetWindowSubclass(list, ListBox_WndProc, IDC_PICTURE_LIST, reinterpret_cast<DWORD_PTR>(&w));

		NONCLIENTMETRICS ncm;
		ncm.cbSize = sizeof(ncm);
		::SystemParametersInfoW(
			SPI_GETNONCLIENTMETRICS,
			sizeof(ncm),
			&ncm,
			0
			);

		font = ::CreateFontIndirectW(
			&ncm.lfCaptionFont);
		FORWARD_WM_SETFONT(list, font, FALSE, ::SendMessageW);

		this->register_object(&button);
		button.set_text(L"Add");
		button.set_callback([&](){
			std::vector<std::wstring> files;
			std::tie(files, index) = win32::get_open_file(
				w,
				{
					{L"Bitmap", {L".bmp"}},
					{L"Portable Network Graphics", {L".png"}},
					{L"Graphics Interchange Format", {L".gif"}},
					{L"JPEG", {L".jpg", L".jpeg"}},
					{L"All Image Files", {L".bmp", L".png", L".gif", L".jpg", L".jpeg"}}
			}, index);
			bool flag = false;
			for(auto &file: files){
				if(w.add_file(file))
					flag = true;
			}
			if(flag && w.get_file_count() != 0)
				w.select_file(w.get_file_count() - 1);
		});
	}

	~picture_list()
	{
		this->unregister_object(&button);
	}

	bool add_file(const std::wstring &s)
	{
		if(ListBox_FindStringExact(list, -1, s.c_str()) == LB_ERR){
			ListBox_AddString(list, s.c_str());
			return true;
		}else
			return false;
	}

	void delete_file(int index)
	{
		ListBox_DeleteString(list, index);
	}

	void select_file(int index)
	{
		ListBox_SetCurSel(list, index);
	}

	void set_size(const direct2d::size &s) override
	{
		this->base::set_size(s);
		button.set_position({20, s.height - 80});
		button.set_size({s.width - 40, 70});
		::SetWindowPos(list, nullptr, 0, 0, static_cast<int>(s.width - 40), static_cast<int>(s.height - 100), SWP_NOMOVE | SWP_NOZORDER);
	}

	void set_position(const direct2d::point &p) override
	{
		this->base::set_position(p);
		if(p.x != pos.x){
			::SetWindowPos(list, nullptr, static_cast<int>(p.x + 20), 10, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			::RedrawWindow(list, nullptr, nullptr, RDW_INTERNALPAINT);
		}
		pos = p;
	}

	void draw(const direct2d::paint_params &pp) override
	{
		if(this->get_size().width > 50.f)
			this->base::draw(pp);
	}
};
