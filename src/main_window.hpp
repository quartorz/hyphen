#pragma once

#include <quote/win32/window.hpp>
#include <quote/win32/creator.hpp>
#include <quote/win32/procs.hpp>
#include <quote/win32/object_processor.hpp>

#include <quote/direct2d/traits.hpp>
#include <quote/direct2d/painter.hpp>
#include <quote/direct2d/scene_manager.hpp>

namespace win32 = quote::win32;
namespace direct2d = quote::direct2d;

#include <memory>

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
		win32::drop_files<main_window>,
		win32::object_processor<main_window, direct2d::traits>,
		direct2d::painter<main_window>,
		ListBoxProcessor<main_window>>,
	public win32::creator<main_window>,
	public direct2d::scene_manager<main_window>
{
	class main_view;

	std::unique_ptr<main_view> view;

	std::vector<std::wstring> files;
	int index = -1;

public:
	static const wchar_t *get_class_name()
	{
		return L"hyphen";
	}

	bool initialize();
	void uninitialize();

	std::size_t get_file_count() const;

	void on_drop_files(const std::vector<std::wstring> &files);
	void select_file(int index);
	void prev_file();
	void next_file();
	bool add_file(const std::wstring &);
	void delete_file();
};

#include "main_view.hpp"

inline bool main_window::initialize()
{
	view = std::make_unique<main_view>(*this);
	this->add_scene(0, view.get());
	this->select_scene(0);

	return true;
}

inline void main_window::uninitialize()
{
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
		select_file(files.size() - 1);
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
