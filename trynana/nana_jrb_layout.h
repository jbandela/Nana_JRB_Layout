// Copyright John R. Bandela 2012. Released under the Boost Software License

#pragma once
#ifndef NANA_JRB_LAYOUT_JRB_09192012
#define NANA_JRB_LAYOUT_JRB_09192012


#include <nana/gui/widgets/widget.hpp>
#include <map>
#include <vector>
#include <string>
#include <memory>
namespace nana_jrb{

	class layout_base{
		int x1;
		int x2;
		int y1;
		int y2;

	public:
		layout_base(int x1, int x2, int y1, int y2){
			this->x1 = x1;
			this->x2 = x2;
			this->y1 = y1;
			this->y2 = y2;

		}
		nana::rectangle get_rectangle(){
			return nana::rectangle(x1,y1,x2-x1,y2-y1);
		}
		nana::point get_upper_left(){
			return nana::point(x1,y1);
		}
		nana::point get_lower_right(){
			return nana::point(x2,y2);
		}

		// Gets a centered rectangle within margins. If the margins are greater than the rectangle returns 0, give height or width -1 for stretch
		nana::rectangle get_centered(nana::rectangle r,int margin_left, int margin_right, int margin_top, int margin_bottom);
	};
	struct widget_dimensions{
		int r;
		int c;
		int width;
		int height;
		int margin_left;
		int margin_right;
		int margin_top;
		int margin_bottom;
		int row_span;
		int column_span;
		widget_dimensions()
			:r(0),c(0),width(-1),height(-1),margin_left(0),margin_right(0),margin_top(0),margin_bottom(0),
			row_span(1),column_span(1)

		{

		}

		widget_dimensions(int p_r,int p_c, int w, int h,int m_l, int m_r, int m_t, int m_b)
			:r(p_r),c(p_c),width(w),height(h),margin_left(m_l),margin_right(m_r),margin_top(m_t),margin_bottom(m_b)
		{

		}

	};
	class grid{
		std::vector<int> row_defs_;
		std::vector<int> column_defs_;
		std::map<nana::gui::widget*,widget_dimensions> widgets_;
		std::map<grid*,widget_dimensions> child_grids_;
	public:

		std::vector<int>& row_defs(){return row_defs_;}
		std::vector<int>& column_defs(){return column_defs_;}


		grid():width_(0),height_(0),x_(0),y_(0){}

		void add_widget(nana::gui::widget& w,widget_dimensions wd){
			widgets_[&w] = wd;
		}
		void add_grid(grid& g,widget_dimensions wd){
			child_grids_[&g] = wd;
		}

		void x(int i){x_ = i;}
		int x(){return x_;}

		void y(int i){
			y_ = i;
		}

		int y(){return y_;}

		void width(int i){
			width_ = i;
		}

		int width(){return width_;}

		void height(int i){
			height_= i;
		}

		int height(){return height_;}

		void update_layout();


	private: 

		int x_;
		int y_;
		int width_;
		int height_;


		int get_row_height(int r,int star);
		int get_column_width(int c,int star);

		std::pair<int,int> get_row_y_height(int r);
		std::pair<int,int> get_column_x_width(int c);

		int get_column_star();
		int get_row_star();


	};





}


#endif //NANA_JRB_LAYOUT_JRB_09192012