#pragma once

#include "main_window.hpp"

#include <quote/direct2d/scene.hpp>
#include <quote/direct2d/vertical_split_scene.hpp>

typedef class main_window::main_view: public direct2d::vertical_split_scene<>
{
	using base = direct2d::vertical_split_scene<>;

	main_window &w;

	class picture_view;
	class picture_list;

	std::unique_ptr<picture_view> view;
	std::unique_ptr<picture_list> list;

public:
	main_view(main_window &);
	~main_view();

	bool add_file(const std::wstring &);
	void delete_file(int);
	void select_file(const std::wstring &, int);

	void set_size(const direct2d::size &) override;
	void draw(const direct2d::paint_params &pp) override;
}main_view_type;

#include "picture_view.hpp"
#include "picture_list.hpp"

inline main_view_type::main_view(main_window &w): w(w)
{
	view = std::make_unique<picture_view>();
	list = std::make_unique<picture_list>(w);

	this->set_left(view.get());
	this->set_right(list.get());
	this->set_bar_color(direct2d::color(255, 255, 255, 80));

	this->add_timer_handler([&](unsigned id){
		auto s = this->get_size();
		auto p = static_cast<float>(this->get_bar_position());
		view->set_size({p, s.height});
		list->set_rect({direct2d::point(p, 0.f), direct2d::size(s.width - p, s.height)});
	}, REDRAW_TIMER);
}

inline main_view_type::~main_view()
{
	this->set_left(nullptr);
	this->set_right(nullptr);
}

inline bool main_view_type::add_file(const std::wstring &s)
{
	return list->add_file(s);
}

inline void main_view_type::delete_file(int index)
{
	list->delete_file(index);
}

inline void main_view_type::select_file(const std::wstring &file, int index)
{
	list->select_file(index);
	view->set_file_name(file);
}

inline void main_view_type::set_size(const direct2d::size &s)
{
	auto size = this->get_size();

	this->base::set_size(s);

	auto p = this->get_bar_position();
	this->set_bar_position(static_cast<int>(static_cast<float>(p) + s.width - size.width));
}

inline void main_view_type::draw(const direct2d::paint_params &pp)
{
	if(w.aero_glass_enabled())
		pp.clear(direct2d::color(0, 0, 0, 0));
	this->base::draw(pp);
}
