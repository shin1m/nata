#include "text.h"
#include "view.h"
#include "search.h"

namespace xemmaix
{

namespace nata
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
	if (!v_instance) t_throwable::f_throw(L"must be inside main.");
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

void f_curses(const t_value& a_callable)
{
	nata::curses::t_session session;
	a_callable();
}

void f_define_pair(short a_pair, short a_fore, short a_back)
{
	if (init_pair(a_pair, a_fore, a_back) == ERR) t_throwable::f_throw(L"init_pair");
}

attr_t f_color_pair(short a_pair)
{
	return COLOR_PAIR(a_pair);
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
	t_type_of<t_proxy>::f_define(this);
	t_type_of<t_text>::f_define(this);
	t_type_of<t_view>::f_define(this);
	t_type_of<t_search>::f_define(this);
	f_define<void(*)(t_extension*, const t_value&), f_main>(this, L"main");
	f_define<void(*)(const t_value&), f_curses>(this, L"curses");
	f_define<void(*)(short, short, short), f_define_pair>(this, L"define_pair");
	f_define<attr_t(*)(short), f_color_pair>(this, L"color_pair");
	a_module->f_put(t_symbol::f_instantiate(L"COLOR_BLACK"), f_as(COLOR_BLACK));
	a_module->f_put(t_symbol::f_instantiate(L"COLOR_RED"), f_as(COLOR_RED));
	a_module->f_put(t_symbol::f_instantiate(L"COLOR_GREEN"), f_as(COLOR_GREEN));
	a_module->f_put(t_symbol::f_instantiate(L"COLOR_YELLOW"), f_as(COLOR_YELLOW));
	a_module->f_put(t_symbol::f_instantiate(L"COLOR_BLUE"), f_as(COLOR_BLUE));
	a_module->f_put(t_symbol::f_instantiate(L"COLOR_MAGENTA"), f_as(COLOR_MAGENTA));
	a_module->f_put(t_symbol::f_instantiate(L"COLOR_CYAN"), f_as(COLOR_CYAN));
	a_module->f_put(t_symbol::f_instantiate(L"COLOR_WHITE"), f_as(COLOR_WHITE));
	a_module->f_put(t_symbol::f_instantiate(L"A_NORMAL"), f_as(A_NORMAL));
	a_module->f_put(t_symbol::f_instantiate(L"A_STANDOUT"), f_as(A_STANDOUT));
	a_module->f_put(t_symbol::f_instantiate(L"A_UNDERLINE"), f_as(A_UNDERLINE));
	a_module->f_put(t_symbol::f_instantiate(L"A_REVERSE"), f_as(A_REVERSE));
	a_module->f_put(t_symbol::f_instantiate(L"A_BLINK"), f_as(A_BLINK));
	a_module->f_put(t_symbol::f_instantiate(L"A_DIM"), f_as(A_DIM));
	a_module->f_put(t_symbol::f_instantiate(L"A_BOLD"), f_as(A_BOLD));
	a_module->f_put(t_symbol::f_instantiate(L"A_ALTCHARSET"), f_as(A_ALTCHARSET));
	a_module->f_put(t_symbol::f_instantiate(L"A_INVIS"), f_as(A_INVIS));
	a_module->f_put(t_symbol::f_instantiate(L"A_PROTECT"), f_as(A_PROTECT));
	a_module->f_put(t_symbol::f_instantiate(L"A_HORIZONTAL"), f_as(A_HORIZONTAL));
	a_module->f_put(t_symbol::f_instantiate(L"A_LEFT"), f_as(A_LEFT));
	a_module->f_put(t_symbol::f_instantiate(L"A_LOW"), f_as(A_LOW));
	a_module->f_put(t_symbol::f_instantiate(L"A_RIGHT"), f_as(A_RIGHT));
	a_module->f_put(t_symbol::f_instantiate(L"A_TOP"), f_as(A_TOP));
	a_module->f_put(t_symbol::f_instantiate(L"A_VERTICAL"), f_as(A_VERTICAL));
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
	a_scan(v_type_proxy);
	a_scan(v_type_text);
	a_scan(v_type_view);
	a_scan(v_type_search);
}

}

}

XEMMAI__MODULE__FACTORY(xemmai::t_object* a_module)
{
	return new xemmaix::nata::t_extension(a_module);
}
