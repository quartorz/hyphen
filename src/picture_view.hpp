#pragma once

#include "main_view.hpp"

#include <quote/direct2d/image.hpp>

class main_view_type::picture_view
	: public direct2d::userdefined_object<main_view_type::picture_view>
{
	using base = direct2d::userdefined_object<main_view_type::picture_view>;

	bool empty = true;
	direct2d::image image;

public:
	picture_view()
	{
		image.set_drawing_mode(direct2d::image::drawing_mode::squeeze);
	}
	void set_file_name(const std::wstring &file)
	{
		empty = file.length() == 0;
		image.set_file_name(file.c_str());
	}
	void set_size(const direct2d::size &s) override
	{
		this->base::set_size(s);
		image.set_size(s);
	}
	void draw(const direct2d::paint_params &pp) override
	{
		if(!empty){
			image.draw(pp);
		}
	}
};
