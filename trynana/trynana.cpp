// Copyright John R. Bandela 2012. Released under the Boost Software License


#include "nana_jrb_layout.h"
#include "nana_jrb_layout_from_file.h"
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <fstream>


#ifdef _DEBUG
#pragma comment(lib,"nana_debug.lib")
#else
#pragma comment(lib,"nana_release.lib")
#endif // DEBUG


int main()
{
	nana::gui::form form;
	form.caption(STR("Hello World"));


	nana_jrb::layout_engine le(form);
	std::ifstream fs("form.txt");
	le.load(fs);


	le.get_widget("btn").make_event<nana::gui::events::click>([](){
		nana::gui::API::exit();
	});

	nana::gui::panel<false> p(form);
	p.size(200,200);
	p.move(50,50);
	nana::gui::listbox list(p);
	list.size(100,100);
	list.append_header(L"hello");
	list.move(-10,10);
	p.hide();


	form.show();
	nana::gui::exec();
} 


