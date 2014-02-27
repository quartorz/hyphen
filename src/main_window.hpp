#pragma once

#include <quote/win32/window.hpp>
#include <quote/win32/creator.hpp>
#include <quote/win32/procs.hpp>
#include <quote/win32/object_processor.hpp>

#include <quote/direct2d/traits.hpp>
#include <quote/direct2d/painter.hpp>
#include <quote/direct2d/scene_manager.hpp>

#include <quote/data.hpp>

namespace win32 = quote::win32;
namespace direct2d = quote::direct2d;

#include <memory>
#include <fstream>

#include <direct.h>

const int REDRAW_TIMER = 100;
const int IDC_PICTURE_LIST = 1000;

#include "listbox_processor.hpp"

class main_window:
	public win32::window<
		main_window,
		win32::quit_on_close<main_window>,
		win32::timer<main_window>,
		win32::keyboard<main_window>,
		win32::resizer<main_window>,
		win32::mover<main_window>,
		win32::drop_files<main_window>,
		win32::object_processor<main_window, direct2d::traits>,
		direct2d::painter<main_window>,
		ListBoxProcessor<main_window>>,
	public win32::creator<main_window>,
	public direct2d::scene_manager<main_window>
{
	class main_view;

	std::unique_ptr<main_view> view;

	std::wstring path;
	int showstate = SW_SHOW, barpos;

	std::vector<std::wstring> files;
	int index = -1;

public:
	static const wchar_t *get_class_name()
	{
		return L"hyphen";
	}

	void show();

	bool initialize();
	void uninitialize();

	void on_move(...)
	{
	}

	std::size_t get_file_count() const;

	void on_drop_files(const std::vector<std::wstring> &files);
	void select_file(int index);
	void prev_file();
	void next_file();
	bool add_file(const std::wstring &);
	void delete_file();
};

#include "main_view.hpp"

inline void main_window::show()
{
	::ShowWindow(this->get_hwnd(), showstate);
	view->set_bar_pos(barpos);
}

inline bool main_window::initialize()
{
	view = std::make_unique<main_view>(*this);
	this->add_scene(0, view.get());
	this->select_scene(0);

	wchar_t path[_MAX_PATH];
	::GetModuleFileNameW(nullptr, path, _MAX_PATH);
	::wcsrchr(path, L'\\')[1] = L'\0';
	this->path = path;
	auto file = std::wifstream((this->path + L"config.dat"), std::ios::in);
	if(file.is_open()){
		quote::data<wchar_t> data = quote::data<wchar_t>::parse(file);
		::MoveWindow(
			this->get_hwnd(),
			std::stoi(data.map()[L"position"].vector()[0].string()),
			std::stoi(data.map()[L"position"].vector()[1].string()),
			std::stoi(data.map()[L"size"].vector()[0].string()),
			std::stoi(data.map()[L"size"].vector()[1].string()),
			FALSE);
		int w, h;
		std::tie(w, h) = this->get_size();
		view->set_size({static_cast<float>(w), static_cast<float>(h)});
		barpos = std::stoi(data.map()[L"bar position"].string());
		showstate = std::stoi(data.map()[L"show state"].string());
	}else{
		this->set_size(800, 600);
		view->set_size({800, 600});
		view->set_bar_pos(600);
		barpos = 600;
	}

	return true;
}

inline void main_window::uninitialize()
{
	std::wofstream ofs((path + L"config.dat"), std::ios::out);
	if(ofs.is_open()){
		quote::data<wchar_t> data;

		data.make_map();

		WINDOWPLACEMENT wp ={};
		wp.length = sizeof wp;
		::GetWindowPlacement(this->get_hwnd(), &wp);

		data.map()[L"show state"].set_string(std::to_wstring(wp.showCmd));

		int w, h;
		std::tie(w, h) = this->get_size();
		data.map()[L"size"].make_vector();
		data.map()[L"size"].vector().emplace_back();
		data.map()[L"size"].vector()[0].set_string(std::to_wstring(wp.rcNormalPosition.right - wp.rcNormalPosition.left));
		data.map()[L"size"].vector().emplace_back();
		data.map()[L"size"].vector()[1].set_string(std::to_wstring(wp.rcNormalPosition.bottom - wp.rcNormalPosition.top));

		data.map()[L"position"].make_vector();
		data.map()[L"position"].vector().emplace_back();
		data.map()[L"position"].vector()[0].set_string(std::to_wstring(wp.rcNormalPosition.left));
		data.map()[L"position"].vector().emplace_back();
		data.map()[L"position"].vector()[1].set_string(std::to_wstring(wp.rcNormalPosition.top));

		data.map()[L"bar position"].set_string(std::to_wstring(view->get_bar_pos()));

		data.save(ofs, true);
	}

	this->remove_scene(0);
}

std::size_t main_window::get_file_count() const
{
	return files.size();
}

void main_window::on_drop_files(const std::vector<std::wstring> &files)
{
	bool flag = false;
	for(auto &file: files){
		if(add_file(file))
			flag = true;
	}
	if(flag && files.size() != 0)
		select_file(this->files.size() - 1);
}

void main_window::select_file(int index)
{
	if(index == this->index)
		return;

	this->index = index;

	if(index == -1){
		view->select_file(L"", -1);
	}else{
		view->select_file(files[index], index);
	}
}

void main_window::prev_file()
{
	if(files.size() != 0){
		if(index == 0)
			select_file(files.size() - 1);
		else
			select_file(index - 1);
	}
}

void main_window::next_file()
{
	if(files.size() != 0){
		if(index == files.size() - 1)
			select_file(0);
		else
			select_file(index + 1);
	}
}

bool main_window::add_file(const std::wstring &file)
{
	auto pos = file.find_last_of(L'.');
	if(pos == std::wstring::npos){
		return false;
	}
	bool flag = false;
	for(auto &ext: {L".bmp", L".png", L".gif", L".jpg", L".jpeg"}){
		if(!::_wcsicmp(&file[pos], ext)){
			flag = true;
			break;
		}
	}
	if(!flag)
		return false;

	if(view->add_file(file))
		this->files.emplace_back(file);

	return true;
}

void main_window::delete_file()
{
	if(index == -1)
		return;

	files.erase(files.begin() + index);
	view->delete_file(index);

	if(files.size() == 0)
		select_file(-1);
	else if(files.size() == index)
		select_file(index - 1);
	else{
		int i = index;
		index = -1;
		select_file(i);
	}
}
