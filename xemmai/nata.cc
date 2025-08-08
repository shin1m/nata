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

void t_library::f_scan(t_scan a_scan)
{
	XEMMAIX__NATA__TYPES(XEMMAI__TYPE__SCAN)
}

std::vector<std::pair<t_root, t_rvalue>> t_library::f_define()
{
	t_type_of<t_proxy>::f_define(this);
	v_type_line.f_construct(f_type<t_object>()->f_derive(t_define{this}
		(L"index"sv)
		(L"from"sv)
		(L"count"sv)
	));
	v_type_span.f_construct(f_type<t_object>()->f_derive(t_define{this}
		(L"from"sv)
		(L"count"sv)
	));
	t_type_of<t_text>::f_define(this);
	t_type_of<t_search>::f_define(this);
	return t_define{this}
	(L"Proxy"sv, static_cast<t_object*>(v_type_proxy))
	(L"Line"sv, static_cast<t_object*>(v_type_line))
	(L"Text"sv, static_cast<t_object*>(v_type_text))
	(L"Span"sv, static_cast<t_object*>(v_type_span))
	(L"Search"sv, static_cast<t_object*>(v_type_search))
	(L"main"sv, t_static<void(*)(const t_pvalue&), [](auto a_callable)
	{
		t_session session;
		a_callable();
	}>())
	;
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_library::t_handle* a_handle)
{
	return xemmai::f_new<xemmaix::nata::t_library>(a_handle);
}
