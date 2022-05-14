#include "search.h"

namespace xemmaix::nata
{

void t_entry::f_dispose()
{
//	std::fprintf(stderr, "dispose %s(%p)\n", typeid(*this).name(), this);
	v_previous->v_next = v_next;
	v_next->v_previous = v_previous;
}

XEMMAI__PORTABLE__THREAD t_session* t_session::v_instance;

#ifdef _WIN32
t_session* t_session::f_instance()
{
	if (!v_instance) f_throw(L"must be inside main."sv);
	return v_instance;
}
#endif

namespace
{

void f_main(t_library* a_library, const t_pvalue& a_callable)
{
	t_session session(a_library);
	a_callable();
}

}

void t_library::f_scan(t_scan a_scan)
{
	a_scan(v_type_proxy);
	a_scan(v_type_text);
	a_scan(v_type_search);
}

std::vector<std::pair<t_root, t_rvalue>> t_library::f_define()
{
	t_type_of<t_proxy>::f_define(this);
	t_type_of<t_text>::f_define(this);
	t_type_of<t_search>::f_define(this);
	return t_define(this)
		(L"Proxy"sv, static_cast<t_object*>(v_type_proxy))
		(L"Text"sv, static_cast<t_object*>(v_type_text))
		(L"Search"sv, static_cast<t_object*>(v_type_search))
		(L"main"sv, t_static<void(*)(t_library*, const t_pvalue&), f_main>())
	;
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_library::t_handle* a_handle)
{
	return xemmai::f_new<xemmaix::nata::t_library>(a_handle);
}
