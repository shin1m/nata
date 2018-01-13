#include "text.h"
#include "view.h"

namespace xemmaix
{

namespace nata
{

void t_entry::f_destroy()
{
//	std::fprintf(stderr, "destroy %s(%p)\n", typeid(*this).name(), this);
	v_previous->v_next = v_next;
	v_next->v_previous = v_previous;
}

XEMMAI__PORTABLE__THREAD t_session* t_session::v_instance;

#ifdef _WIN32
t_session* t_session::f_instance()
{
	if (!v_instance) t_throwable::f_throw(L"must be inside main.");
	return v_instance;
}
#endif

void t_proxy::f_destroy()
{
	v_object = nullptr;
	t_entry::f_destroy();
}

namespace
{

void f_main(t_extension* a_extension, const t_value& a_callable)
{
	t_session session(a_extension);
	a_callable();
}

void f_curses(const t_value& a_callable)
{
	nata::curses::t_session session;
	init_pair(1, COLOR_WHITE, -1);
	init_pair(2, COLOR_BLACK, COLOR_WHITE);
	init_pair(4, -1, COLOR_YELLOW);
	a_callable();
}

intptr_t f_get()
{
	wint_t c;
	if (get_wch(&c) == ERR) t_throwable::f_throw(L"get_wch");
	return c;
}

}

t_extension::t_extension(t_object* a_module) : xemmai::t_extension(a_module)
{
	t_type_of<t_text>::f_define(this);
	t_type_of<t_view>::f_define(this);
	f_define<void(*)(t_extension*, const t_value&), f_main>(this, L"main");
	f_define<void(*)(const t_value&), f_curses>(this, L"curses");
	f_define<intptr_t(*)(), f_get>(this, L"get");
	a_module->f_put(t_symbol::f_instantiate(L"KEY_RESIZE"), f_as(KEY_RESIZE));
	a_module->f_put(t_symbol::f_instantiate(L"KEY_DOWN"), f_as(KEY_DOWN));
	a_module->f_put(t_symbol::f_instantiate(L"KEY_UP"), f_as(KEY_UP));
	a_module->f_put(t_symbol::f_instantiate(L"KEY_LEFT"), f_as(KEY_LEFT));
	a_module->f_put(t_symbol::f_instantiate(L"KEY_RIGHT"), f_as(KEY_RIGHT));
	a_module->f_put(t_symbol::f_instantiate(L"KEY_BACKSPACE"), f_as(KEY_BACKSPACE));
	a_module->f_put(t_symbol::f_instantiate(L"KEY_F1"), f_as(KEY_F(1)));
	a_module->f_put(t_symbol::f_instantiate(L"KEY_F2"), f_as(KEY_F(2)));
	a_module->f_put(t_symbol::f_instantiate(L"KEY_F3"), f_as(KEY_F(3)));
	a_module->f_put(t_symbol::f_instantiate(L"KEY_F4"), f_as(KEY_F(4)));
	a_module->f_put(t_symbol::f_instantiate(L"KEY_F5"), f_as(KEY_F(5)));
	a_module->f_put(t_symbol::f_instantiate(L"KEY_F6"), f_as(KEY_F(6)));
	a_module->f_put(t_symbol::f_instantiate(L"KEY_F7"), f_as(KEY_F(7)));
	a_module->f_put(t_symbol::f_instantiate(L"KEY_F8"), f_as(KEY_F(8)));
	a_module->f_put(t_symbol::f_instantiate(L"KEY_F9"), f_as(KEY_F(9)));
	a_module->f_put(t_symbol::f_instantiate(L"KEY_F10"), f_as(KEY_F(10)));
	a_module->f_put(t_symbol::f_instantiate(L"KEY_F11"), f_as(KEY_F(11)));
	a_module->f_put(t_symbol::f_instantiate(L"KEY_F12"), f_as(KEY_F(12)));
	a_module->f_put(t_symbol::f_instantiate(L"KEY_ENTER"), f_as(KEY_ENTER));
}

void t_extension::f_scan(t_scan a_scan)
{
	a_scan(v_type_text);
	a_scan(v_type_view);
}

}

}

XEMMAI__MODULE__FACTORY(xemmai::t_object* a_module)
{
	return new xemmaix::nata::t_extension(a_module);
}
