#pragma once

#include "main_window.hpp"

#include <quote/direct2d/scene.hpp>

typedef class main_window::main_view: public direct2d::scene<>
{
	class picture_view;
	class picture_list;
	class resize_bar;

	std::unique_ptr<picture_view> view;
	std::unique_ptr<picture_list> list;
	std::unique_ptr<resize_bar> bar;

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
#include "resize_bar.hpp"

inline main_view_type::main_view(main_window &w)
{
	view = std::make_unique<picture_view>();
	list = std::make_unique<picture_list>(w);

	bar = std::make_unique<resize_bar>();
	bar->set_position({100, 0});

	this->register_object(view.get());
	this->register_object(list.get());
	this->register_object(bar.get());

	this->reorder_object(bar.get(), nullptr);

	this->add_timer_handler([&](unsigned id){
		auto s = this->get_size();
		auto p = bar->get_position();
		view->set_size({p.x, s.height});
		list->set_rect({direct2d::point(p.x, 0.f), direct2d::size(s.width - p.x, s.height)});
	}, REDRAW_TIMER);
}

inline main_view_type::~main_view()
{
	this->unregister_object(view.get());
	this->unregister_object(list.get());
	this->unregister_object(bar.get());
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

	this->direct2d::scene<>::set_size(s);

	bar->set_parent_width(s.width);

	auto p = bar->get_position();
	bar->set_position({p.x + s.width - size.width, 0});
	bar->set_size({3, s.height});
}

inline void main_view_type::draw(const direct2d::paint_params &pp)
{
	pp.target->Clear();
	this->direct2d::scene<>::draw(pp);
}
