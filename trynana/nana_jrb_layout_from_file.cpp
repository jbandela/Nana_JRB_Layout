// Copyright John R. Bandela 2012. Released under the Boost Software License


#include "nana_jrb_layout_from_file.h"
#include <JRBJson/json.h>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/checkbox.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/date_chooser.hpp>
#include <nana/gui/widgets/frame.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/picture.hpp>
#include <nana/gui/widgets/progress.hpp>
#include <nana/gui/widgets/slider.hpp>
#include <nana/gui/widgets/textbox.hpp>

//left out menu/menubar/scroll/tabbar/categorize


#include <stack>

namespace{
	struct layout_parser{
		nana_jrb::layout_engine* le;
		std::stack<std::string> grid_stack;

		void handle_widget(jrb_json::value& v){
			std::string type = v[std::string("type")].get<std::string>();
			std::string name = v[std::string("name")].get<std::string>();
			le->add_widget(name,type);
			auto& w = le->get_widget(name);
			auto& g = le->get_grid(grid_stack.top());

			jrb_json::object_type& ot = v.get_exact<jrb_json::object_type>();

			nana_jrb::widget_dimensions wd;

			handle_widget_dimensions(ot, v, wd);



			g.add_widget(w,wd);

			if(ot.count("caption")){
				jrb_json::value& caption = v["caption"];
				nana::charset ch = caption.get<std::string>();
				w.caption(ch);
			}
			if(ot.count("background")){
				jrb_json::value& background = v["background"];
				char color[3] = {0};
				for(int i = 0; i < background.size(); i++){
					color[i] = background[i].get<int>();
				}
				w.background(nana::make_rgb(color[0],color[1],color[2]));
			}
			if(ot.count("foreground")){
				jrb_json::value& foreground = v["foreground"];
				char color[3] = {0};
				for(int i = 0; i < foreground.size(); i++){
					color[i] = foreground[i].get<int>();
				}
				w.foreground(nana::make_rgb(color[0],color[1],color[2]));
			}
			bool visible = true;
			if(ot.count("visible")){
				visible = v["visible"].get<bool>();
			}

			if(visible){
				w.show();
			}
			else{
				w.hide();
			}

			bool enabled = true;
			if(ot.count("enabled")){
				enabled = v["enabled"].get<bool>();
			}

			w.enabled(enabled);

			if(ot.count("typeface")){
				jrb_json::value& typeface = v["typeface"];
				std::string fontname; unsigned int size=0;
				bool bold = false;
				bool italic = false;
				bool underline = false;
				bool strike_out = false;
				auto& tot = typeface.get_exact<jrb_json::object_type>();
				if(tot.count("name")){
					fontname = typeface["name"].get<std::string>();
				}
				if(tot.count("size")){
					size = typeface["size"].get<unsigned int>();
				}
				if(tot.count("bold")){
					bold = typeface["bold"].get<bool>();
				}
				if(tot.count("italic")){
					italic = typeface["italic"].get<bool>();
				}
				if(tot.count("underline")){
					underline = typeface["underline"].get<bool>();
				}
				if(tot.count("strike_out")){
					strike_out = typeface["strike_out"].get<bool>();
				}
				nana::string sfontname = nana::charset(fontname);
				nana::paint::font f(sfontname.c_str(),size,bold,italic,underline,strike_out);
				w.typeface(f);
			}

			// handle custom type processing
			if(nana_jrb::layout_engine::property_processor().count(type)){
				nana_jrb::layout_engine::property_processor()[type](w,v);
			}
		}	

		void handle_widget_dimensions( jrb_json::object_type &ot, jrb_json::value& v, nana_jrb::widget_dimensions &wd ) 
		{
			if(ot.count("r")){
				jrb_json::value& r = v["r"];
				wd.r = r.get<int>();
			}
			if(ot.count("c")){
				jrb_json::value& c = v["c"];
				wd.c = c.get<int>();
			}
			if(ot.count("width")){
				jrb_json::value& width = v["width"];
				wd.width = width.get<int>();
			}
			if(ot.count("height")){
				jrb_json::value& height = v["height"];
				wd.height = height.get<int>();
			}
			if(ot.count("margin_left")){
				jrb_json::value& margin_left = v["margin_left"];
				wd.margin_left = margin_left.get<int>();
			}
			if(ot.count("margin_right")){
				jrb_json::value& margin_right = v["margin_right"];
				wd.margin_right = margin_right.get<int>();
			}
			if(ot.count("margin_top")){
				jrb_json::value& margin_top = v["margin_top"];
				wd.margin_top = margin_top.get<int>();
			}
			if(ot.count("margin_bottom")){
				jrb_json::value& margin_bottom = v["margin_bottom"];
				wd.margin_bottom = margin_bottom.get<int>();
			}
			if(ot.count("column_span")){
				jrb_json::value& column_span = v["column_span"];
				wd.column_span = column_span.get<int>();
			}
			if(ot.count("row_span")){
				jrb_json::value& row_span = v["row_span"];
				wd.row_span = row_span.get<int>();
			}
		}

		void handle_child(jrb_json::value& v){
			std::string type = v[std::string("type")].get<std::string>();
			std::string name = v[std::string("name")].get<std::string>();
			if(type=="grid"){
				handle_grid(v);
			}else{
				handle_widget(v);
			}
		}


		void handle_grid(jrb_json::value& v){
			std::string type = v[std::string("type")].get<std::string>();
			std::string name = v[std::string("name")].get<std::string>();

			// set the root grid
			le->set_root_grid_if_not_set(name);

			le->add_grid(name,nana_jrb::grid());
			auto& g = le->get_grid(name);
			std::string prev_grid;
			if(grid_stack.size()){
				prev_grid = grid_stack.top();
			}
			grid_stack.push(name);
			jrb_json::object_type& ot = v.get_exact<jrb_json::object_type>();

			// get row defs
			if(ot.count("row_defs")){
				jrb_json::value& rd = v["row_defs"];
				for(int i = 0; i < rd.size(); i++){
					int val = rd[i].get<int>();
					g.row_defs().push_back(val);
				}
			}
			if(ot.count("column_defs")){
				jrb_json::value& cd = v["column_defs"];
				for(int i = 0; i < cd.size(); i++){
					int val = cd[i].get<int>();
					g.column_defs().push_back(val);
				}
			}
			if(ot.count("x")){
				jrb_json::value& x = v["x"];
				g.x(x.get<int>());
			}
			if(ot.count("y")){
				jrb_json::value& y = v["y"];
				g.y(y.get<int>());
			}
			if(ot.count("width")){
				jrb_json::value& width = v["width"];
				g.width(width.get<int>());
			}
			if(ot.count("height")){
				jrb_json::value& height = v["height"];
				g.height(height.get<int>());
			}
			// widget dimensions
			nana_jrb::widget_dimensions wd;

			handle_widget_dimensions(ot,v,wd);

			if(prev_grid.size()){
				le->get_grid(prev_grid).add_grid(g,wd);
			}



			//
			if(ot.count("children")){
				jrb_json::value& cd = v["children"];
				for(int i = 0; i < cd.size(); i++){
					auto val = cd[i];
					handle_child(val);
					
				}
			}



			grid_stack.pop();

		}

	};


}

void nana_jrb::layout_engine::load( std::istream& is )
{
	std::string data;
	while(is){
		std::string line;
		getline(is,line);
		data += "\n";
		data += line;

	}

	jrb_json::value v =	jrb_json::parse_json(data);
	layout_parser p;
	p.le = this;
	p.handle_grid(v);

	auto& g = get_grid(root_grid_);



	// set height of the root grid based on the form
	g.height(f_->size().height);
	g.width(f_->size().width);

	g.update_layout();

	// handle the resize event
	f_->make_event<nana::gui::events::size>([&g,this](){
		// prevents a crash when size is too small
		if(f_->size().width > 20 && f_->size().height > 20){
			g.width(f_->size().width);
			g.height(f_->size().height);
			g.update_layout();

		}
	});

}

namespace{
	template<class T>
	std::unique_ptr<T> make_unique(T* t){return std::unique_ptr<T>(t);}

}
nana_jrb::layout_engine::layout_engine(nana::gui::form& f):f_(&f)
{
}

std::map<std::string,std::function<std::unique_ptr<nana::gui::widget>(nana::gui::window)>>& nana_jrb::layout_engine::type_factory()
{
	static std::map<std::string,std::function<std::unique_ptr<nana::gui::widget>(nana::gui::window)>> tf;

	// default handlers
	if(tf.size()==0){
		tf["button"] = [](nana::gui::window f){return make_unique(new nana::gui::button(f));};
		tf["label"] = [](nana::gui::window f){return make_unique( new nana::gui::label(f));};
		tf["checkbox"] = [](nana::gui::window f){return make_unique( new nana::gui::checkbox(f));};
		tf["combox"] = [](nana::gui::window f){return make_unique( new nana::gui::combox(f));};
		tf["date_chooser"] = [](nana::gui::window f){return make_unique(new nana::gui::date_chooser(f));};
		tf["frame"] = [](nana::gui::window f){return make_unique( new nana::gui::frame(f));};
		tf["listbox"] = [](nana::gui::window f){return make_unique(new nana::gui::listbox(f));};
		tf["picture"] = [](nana::gui::window f){return make_unique(new nana::gui::picture(f));};
		tf["progress"] = [](nana::gui::window f){return make_unique( new nana::gui::progress(f));};
		tf["slider"] = [](nana::gui::window f){return make_unique( new nana::gui::slider(f));};
		tf["textbox"] = [](nana::gui::window f){return make_unique(new nana::gui::textbox(f));};
	}
	return tf;

}

std::map<std::string,std::function<void (nana::gui::widget&,jrb_json::value&)>>& nana_jrb::layout_engine::property_processor()
{
	static std::map<std::string,std::function<void (nana::gui::widget&,jrb_json::value&)>> pp;

	// Add the default processor
	if(pp.size()==0){
		auto label_handler = [](nana::gui::widget& w,jrb_json::value& v){
			nana::gui::label& lbl = dynamic_cast<nana::gui::label&>(w);
			auto& ot = v.get_exact<jrb_json::object_type>();
			if(ot.count("format")){
				lbl.format(v["format"].get<bool>());
			}
			if(ot.count("transparent")){
				lbl.transparent(v["transparent"].get<bool>());
			}

		};
		pp["label"] = label_handler;

		auto button_handler = [](nana::gui::widget& w,jrb_json::value& v){
			nana::gui::button& button = dynamic_cast<nana::gui::button&>(w);
			auto& ot = v.get_exact<jrb_json::object_type>();
			if(ot.count("enable_focus_color")){
				button.enable_focus_color(v["enable_focus_color"].get<bool>());
			}
			if(ot.count("enable_pushed")){
				button.enable_pushed(v["enable_pushed"].get<bool>());
			}
			if(ot.count("omitted")){
				button.omitted(v["omitted"].get<bool>());
			}
			if(ot.count("icon")){
				nana::string s = nana::charset(v["icon"].get<std::string>());
				nana::paint::image img(s);
				button.icon(img);
			}
			if(ot.count("image")){
				nana::string s = nana::charset(v["image"].get<std::string>());
				nana::paint::image img(s);
				button.image(img);
			}

		};
		pp["button"] = button_handler;


		auto checkbox_handler = [](nana::gui::widget& w,jrb_json::value& v){
			nana::gui::checkbox& check = dynamic_cast<nana::gui::checkbox&>(w);
			auto& ot = v.get_exact<jrb_json::object_type>();
			if(ot.count("transparent")){
				check.transparent(v["transparent"].get<bool>());
			}

		};
		pp["checkbox"] = checkbox_handler;

		auto combox_handler = [](nana::gui::widget& w,jrb_json::value& v){
			nana::gui::combox& combox = dynamic_cast<nana::gui::combox&>(w);
			auto& ot = v.get_exact<jrb_json::object_type>();
			if(ot.count("editable")){
				combox.editable(v["editable"].get<bool>());
			}
			if(ot.count("items")){
				auto items = v["items"];
				for(int i = 0; i < items.size(); i++){
					combox.push_back(nana::charset(items[i].get<std::string>()));
				}
			}



		};
		pp["combox"] = combox_handler;

		// datechooser needs no further processing

		// frame needs no further processing

		auto listbox_handler = [](nana::gui::widget& w,jrb_json::value& v){
			nana::gui::listbox& listbox = dynamic_cast<nana::gui::listbox&>(w);
			auto& ot = v.get_exact<jrb_json::object_type>();
			if(ot.count("checkable")){
				listbox.checkable(v["checkable"].get<bool>());
			}
			if(ot.count("show_header")){
				listbox.show_header(v["show_header"].get<bool>());
			}
			if(ot.count("items")){
				auto items = v["items"];
				for(int i = 0; i < items.size(); i++){
					nana::string s = nana::charset(items[i].get<std::string>());
					listbox.append_item(s);
				}
			}



		};
		pp["listbox"] = listbox_handler;


		auto picture_handler = [](nana::gui::widget& w,jrb_json::value& v){
			nana::gui::picture& picture = dynamic_cast<nana::gui::picture&>(w);
			auto& ot = v.get_exact<jrb_json::object_type>();
			if(ot.count("transparent")){
				picture.transparent(v["transparent"].get<bool>());
			}
			if(ot.count("image")){
				nana::string s = nana::charset(v["image"].get<std::string>());
				nana::paint::image img(s);
				picture.load(img);
			}



		};
		pp["picture"] = picture_handler;


		auto progress_handler = [](nana::gui::widget& w,jrb_json::value& v){
			nana::gui::progress& progress = dynamic_cast<nana::gui::progress&>(w);
			auto& ot = v.get_exact<jrb_json::object_type>();
			if(ot.count("unknown")){
				progress.unknown(v["unknown"].get<bool>());
			}
			if(ot.count("amount")){
				progress.amount(v["amount"].get<unsigned int>());
			}
			if(ot.count("value")){
				progress.value(v["value"].get<unsigned int>());
			}




		};
		pp["progress"] = progress_handler;




		auto slider_handler = [](nana::gui::widget& w,jrb_json::value& v){
			nana::gui::slider& slider = dynamic_cast<nana::gui::slider&>(w);
			auto& ot = v.get_exact<jrb_json::object_type>();
			if(ot.count("transparent")){
				slider.transparent(v["transparent"].get<bool>());
			}
			if(ot.count("vertical")){
				slider.vertical(v["vertical"].get<bool>());
			}
			if(ot.count("vmax")){
				slider.vmax(v["vmax"].get<unsigned int>());
			}
			if(ot.count("value")){
				slider.value(v["value"].get<unsigned int>());
			}




		};
		pp["slider"] = slider_handler;

		auto textbox_handler = [](nana::gui::widget& w,jrb_json::value& v){
			nana::gui::textbox& textbox = dynamic_cast<nana::gui::textbox&>(w);
			auto& ot = v.get_exact<jrb_json::object_type>();
			if(ot.count("multi_lines")){
				textbox.multi_lines(v["multi_lines"].get<bool>());
			}
			if(ot.count("editable")){
				textbox.editable(v["editable"].get<bool>());
			}
			if(ot.count("border")){
				textbox.border(v["border"].get<bool>());

			}
			if(ot.count("tip_string")){
				textbox.tip_string(nana::charset(v["tip_string"].get<std::string>()));
			}




		};
		pp["textbox"] = textbox_handler;





	}
	return pp;
}

void nana_jrb::layout_engine::add_type( std::string s, std::function<std::unique_ptr<nana::gui::widget>(nana::gui::window)> f )
{
	type_factory()[s] = f;
}

void nana_jrb::layout_engine::add_property_processor( std::string type,std::function<void (nana::gui::widget&,jrb_json::value&)> f )
{
	property_processor()[type] = f;
}
