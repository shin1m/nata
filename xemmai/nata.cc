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
	if (!v_instance) f_throw(L"must be inside main.");
	return v_instance;
}
#endif

namespace
{

void f_main(t_extension* a_extension, const t_value& a_callable)
{
	t_session session(a_extension);
	a_callable();
}

}

t_extension::t_extension(t_object* a_module) : xemmai::t_extension(a_module)
{
	t_type_of<t_proxy>::f_define(this);
	t_type_of<t_text>::f_define(this);
	t_type_of<t_search>::f_define(this);
	f_define<void(*)(t_extension*, const t_value&), f_main>(this, L"main");
}

void t_extension::f_scan(t_scan a_scan)
{
	a_scan(v_type_proxy);
	a_scan(v_type_text);
	a_scan(v_type_search);
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_object* a_module)
{
	return new xemmaix::nata::t_extension(a_module);
}
