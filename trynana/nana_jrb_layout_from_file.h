// Copyright John R. Bandela 2012. Released under the Boost Software License


#pragma once
#ifndef NANA_JRB_LAYOUT_FROM_FILE_JRB_09192012
#define NANA_JRB_LAYOUT_FROM_FILE_JRB_09192012

#include <functional>
#include <memory>
#include "nana_jrb_layout.h"
#include <map>
#include <istream>
#include <exception>
#include <string>
#include <nana/gui/wvl.hpp>
#include <JRBJson/json.h>

namespace nana_jrb{

	struct bad_layout_format:public std::exception{
		bad_layout_format(std::string s):std::exception(s.c_str()){}

	};

	class layout_engine{
		std::map<std::string,std::unique_ptr<nana::gui::widget>> widgets_;
		std::map<std::string,grid> grids_;
		nana::gui::form* f_;
		std::string root_grid_;
	public:
		layout_engine(nana::gui::form& f);
		void load(std::istream& is);
		static void add_type(std::string s, std::function<std::unique_ptr<nana::gui::widget>(nana::gui::window)> f);
		void set_root_grid_if_not_set(std::string s){if(root_grid_.size()==0)root_grid_ = s;}

		static void add_property_processor(std::string type,std::function<void (nana::gui::widget&,jrb_json::value&)> f);
		void add_widget(std::string name, std::string type){
			widgets_[name] = std::move(type_factory()[type](*f_));
		}
		void add_grid(std::string s,const grid& g){
			grids_[s] = g;
		}
		grid& get_grid(std::string s){return grids_[s];}
		nana::gui::widget& get_widget(std::string s){return *widgets_[s];}
		template<class Widget_Type>
		Widget_Type& get(std::string s){
			
			Widget_Type* w =  dynamic_cast<Widget_Type*>(widgets_[s].get());
			if(!w)throw std::runtime_error("Incorrect Widget Type");
			return *w;
		}

		static std::map<std::string,std::function<std::unique_ptr<nana::gui::widget>(nana::gui::window)>>& type_factory();
		static std::map<std::string,std::function<void (nana::gui::widget&,jrb_json::value&)>>&  property_processor();
	};

}

#endif //NANA_JRB_LAYOUT_FROM_FILE_JRB_09192012