// Copyright John R. Bandela 2012. Released under the Boost Software License


#include "nana_jrb_layout.h"
#include <iostream>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/scroll.hpp>

nana::rectangle nana_jrb::layout_base::get_centered( nana::rectangle r,int margin_left, int margin_right, int margin_top, int margin_bottom )
{
	int x1m = x1 + margin_left;
	int y1m = y1 + margin_top;
	int x2m = x2 - margin_bottom;
	int y2m = y2 - margin_bottom;

	int width = x2m-x1m;
	int height = y2m-y1m;
	if(width <= 0 || height <= 0 ){
		return nana::rectangle(x1m, y1m,0,0);
	}

	nana::rectangle ret;

	if(r.width == -1 || width <= r.width){
		ret.width = width;
		ret.x = x1m;
	}else{
		int dx = (width - r.width)/2;
		ret.x = x1m + dx;
		ret.width = r.width;
	}
	if(r.height == -1 || height <= r.height){
		ret.height = height;
		ret.y = y1m;
	}else{
		int dy = (height - r.height)/2;
		ret.y = y1m + dy;
		ret.height = r.height;
	}

	return ret;
}

void nana_jrb::grid::update_layout()
{
	base_window_->move(x_,y_,width_,height_);
	
	//get the row and column sizes before scrollbars
	int rows = row_defs_.size();
	int cols = column_defs_.size();

	// add scroll to grid if needed
	add_scroll_to_grid_if_needed();

	for(auto& p:widgets_){
		widget_dimensions wd = p.second;
		auto x_width = get_column_x_width(wd.c);
		auto y_height = get_row_y_height(wd.r);
		int cstar = get_column_star();
		for(int i = 1; i < wd.column_span &&  (wd.c + i < cols); i++){
			x_width.second += get_column_width(wd.c + 1,cstar);
		}
		int rstar = get_row_star();
		for(int i = 1; i < wd.row_span &&  (wd.r + i < rows); i++){
			y_height.second += get_row_height(wd.r + 1,rstar);
		}
		layout_base base(x_width.first,x_width.first + x_width.second,y_height.first,y_height.first + y_height.second);
		auto rect = base.get_centered(nana::rectangle(0,0,wd.width,wd.height),wd.margin_left,wd.margin_right,wd.margin_top,wd.margin_bottom);
		
		p.first->move(rect.x,rect.y,rect.width,rect.height);
	}
	for(auto& p:child_grids_){
		widget_dimensions wd = p.second;
		auto x_width = get_column_x_width(wd.c);
		auto y_height = get_row_y_height(wd.r);

		p.first->x(x_width.first);
		p.first->width(x_width.second);

		p.first->y(y_height.first);
		p.first->height(y_height.second);

		p.first->update_layout();
	}

}

int nana_jrb::grid::get_row_height( int r,int star )
{
	if(r >= row_defs_.size()){
		return 0;
	}
	int rd = row_defs_.at(r);
	if(rd < 0){
		int rw = rd/-1;
		return rw*star;
	}else{
		return rd;
	}
}

int nana_jrb::grid::get_column_width( int c,int star )
{
	if(c >= column_defs_.size()){
		return 0;
	}
	int cd = column_defs_.at(c);
	if(cd < 0){
		int ch = cd/-1;
		return ch*star;
	}else{
		return cd;
	}
}

std::pair<int,int> nana_jrb::grid::get_row_y_height( int r )
{
	if(r >= row_defs_.size()){
		return std::make_pair(0,0);
	}
	int star = get_row_star();
	int y = get_starting_y();
	for(int i = 0; i < r; i++){
		y += get_row_height(i,star);
	}

	return std::make_pair(y,get_row_height(r,star));
}

std::pair<int,int> nana_jrb::grid::get_column_x_width( int c )
{
	if(c >= column_defs_.size()){
		return std::make_pair(0,0);
	}
	int star = get_column_star();
	int x = get_starting_x();
	for(int i = 0; i < c; i++){
		x += get_column_width(i,star);
	}

	return std::make_pair(x,get_column_width(c,star));
}

int nana_jrb::grid::get_column_star()
{
	int w = get_effective_width();
	int star = 0;
	for(int r:column_defs_){
		if(r < 0){
			int s = r/-1;
			star += s;
		}else{
			w -= r;
		}
	}
	if(star == 0 ){
		return 0;
	}
	if(w <= 0){
		return 0;
	}
	return w/star;
}

int nana_jrb::grid::get_row_star()
{
	int h = get_effective_height();
	int star = 0;
	for(int c:row_defs_){
		if(c < 0){
			int s = c/-1;
			star += s;
		}else{
			h -= c;
		}
	}
	if(star == 0 ){
		return 0;
	}
	if(h <= 0){
		return 0;
	}
	return h/star;
}

nana_jrb::grid::grid(nana::gui::widget& parent) :width_(0),height_(0),x_(0),y_(0),base_window_(new nana::gui::panel<true>(parent)),min_width_(0),min_height_(0)
{
}

void nana_jrb::grid::min_width( int i )
{
	min_width_ = i;
	if(!h_scroll_){
		h_scroll_.reset(new nana::gui::scroll<false>(base_window().parent(),nana::rectangle(0,0,0,0)));
		h_scroll_->make_event<nana::gui::events::mouse_down>([this](){
			update_layout();
		});
		h_scroll_->make_event<nana::gui::events::mouse_wheel>([this](){
			update_layout();
		});
		h_scroll_->make_event<nana::gui::events::mouse_move>([this](const nana::gui::eventinfo& e){
			if(e.mouse.left_button)
				update_layout();
		});

		auto& hs = dynamic_cast<nana::gui::scroll<false>&>(*h_scroll_);

		hs.amount(100);
	}

}

void nana_jrb::grid::min_height( int i )
{
	min_height_= i;
	if(!v_scroll_){
		v_scroll_.reset(new nana::gui::scroll<true>(base_window().parent(),nana::rectangle(0,0,0,0)));
		v_scroll_->make_event<nana::gui::events::mouse_down>([this](){
			update_layout();
		});
		v_scroll_->make_event<nana::gui::events::mouse_wheel>([this](){
			update_layout();
		});
		v_scroll_->make_event<nana::gui::events::mouse_move>([this](const nana::gui::eventinfo& e){
			if(e.mouse.left_button)
				update_layout();
		});
		auto& vs = dynamic_cast<nana::gui::scroll<true>&>(*v_scroll_);

		vs.amount(100);

	}
}

void nana_jrb::grid::add_scroll_to_grid_if_needed()
{

	bool v_scroll_needed = (min_height() && min_height() > height());

	bool h_scroll_needed = (min_width() && min_width() > width());
	if(h_scroll_needed){
		// add a horizontal scroll bar
		auto prev_size = base_window().size();
		auto prev_pos = base_window().pos();
		base_window().size(prev_size.width,prev_size.height-20);
		h_scroll_->move(prev_pos.x,prev_pos.y + prev_size.height-20,prev_size.width - v_scroll_needed*20,20);

		auto& sc = dynamic_cast<nana::gui::scroll<false>&>(*h_scroll_);

		sc.amount(min_width());
		sc.range( width()  );


		h_scroll_->show();
	}else{
		base_window().size(base_window().size().width,height());
		if(h_scroll_)
			h_scroll_->hide();

	}
	if(v_scroll_needed){
		// add a vertical scroll bar
		auto prev_size = base_window().size();
		auto prev_pos = base_window().pos();
		base_window().size(prev_size.width-20,prev_size.height);
		v_scroll_->move(prev_pos.x + prev_size.width-20,prev_pos.y,20,prev_size.height);
		v_scroll_->show();

		auto& sc = dynamic_cast<nana::gui::scroll<true>&>(*v_scroll_);

		sc.amount(min_height());
		sc.range( height()  );


	}else
	{
		base_window().size(width(),base_window().size().height);
		if(v_scroll_)
			v_scroll_->hide();
	}


}


int nana_jrb::grid::get_starting_y()
{


	if(min_height() && min_height() > height()){
		auto& vs = dynamic_cast<nana::gui::scroll<true>&>(*v_scroll_);
		
		return -vs.value();
		

	}else{
		return 0;
	}

}

int nana_jrb::grid::get_starting_x()
{
	if(min_width() && min_width() > width()){
		auto& hs = dynamic_cast<nana::gui::scroll<false>&>(*h_scroll_);

		return -hs.value();


	}else{
		return 0;
	}

}
