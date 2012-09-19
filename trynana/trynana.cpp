// Copyright John R. Bandela 2012. Released under the Boost Software License


#include "nana_jrb_layout.h"
#include "nana_jrb_layout_from_file.h"
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

	form.show();
	nana::gui::exec();
} 


