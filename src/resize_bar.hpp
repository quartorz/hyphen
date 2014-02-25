#pragma once

#include "main_view.hpp"

class main_view_type::resize_bar: public direct2d::userdefined_object<resize_bar>{
	using base = direct2d::userdefined_object<resize_bar>;

	direct2d::solid_brush brush;

	float max;
	bool pushing = false;

public:
	resize_bar()
	{
		this->register_resource(&brush);
		brush.set_color({0, 0, 0});
	}
	~resize_bar()
	{
		this->unregister_resource(&brush);
	}
	void set_parent_width(float m)
	{
		max = m - 20;
	}
	void draw(const direct2d::paint_params &pp) override
	{
		auto rect = this->get_rect();
/*		pp.draw_line(
			brush,
			direct2d::line(
				{rect.left + rect.width() / 2, 0},
				{rect.left + rect.width() / 2, rect.bottom}),
			rect.width() / 2);*/
		rect.left += 1;
		rect.right -= 1;
		pp.target->PushAxisAlignedClip(rect, D2D1_ANTIALIAS_MODE_ALIASED);
		pp.target->Clear();
		pp.target->PopAxisAlignedClip();
	}
	bool is_colliding(const direct2d::point &ap) override
	{
		return ap.is_inside(this->get_rect());
	}
	void on_mouse_move(const direct2d::point &p, hittest &ht) override
	{
		this->base::on_mouse_move(p, ht);

		ht.set_cursor(hittest::cursor::leftright);
		if(pushing){
			this->set_position(p);
		}
	}
	void on_left_press(const direct2d::point &, hittest &ht) override
	{
		ht.set_cursor(hittest::cursor::leftright);
		pushing = true;
	}
	void on_left_release(const direct2d::point &, hittest &ht) override
	{
		ht.set_cursor(hittest::cursor::leftright);
		pushing = false;
	}
	void set_position(const direct2d::point &p) override
	{
		this->base::set_position({std::max(20.f, std::min(p.x, max)), 0.f});
	}
};