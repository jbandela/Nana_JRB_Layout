// Copyright John R. Bandela 2012. Released under the Boost Software License


#include "nana_jrb_layout.h"
#include <iostream>

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
	for(auto& p:widgets_){
		widget_dimensions wd = p.second;
		auto x_width = get_column_x_width(wd.c);
		auto y_height = get_row_y_height(wd.r);
		int cstar = get_column_star();
		for(int i = 1; i < wd.column_span &&  (wd.c + i < column_defs_.size()); i++){
			x_width.second += get_column_width(wd.c + 1,cstar);
		}
		int rstar = get_row_star();
		for(int i = 1; i < wd.row_span &&  (wd.r + i < row_defs_.size()); i++){
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
	int y = y_;
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
	int x = x_;
	for(int i = 0; i < c; i++){
		x += get_column_width(i,star);
	}

	return std::make_pair(x,get_column_width(c,star));
}

int nana_jrb::grid::get_column_star()
{
	int w = width_;
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
	int h = height_;
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
