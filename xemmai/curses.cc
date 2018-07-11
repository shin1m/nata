#include "curses.h"

namespace xemmai
{

void t_type_of<xemmaix::nata::curses::t_token>::f_define(t_extension* a_extension)
{
	using xemmaix::nata::curses::t_token;
	t_define<t_token, t_object>(a_extension, L"Token")
		(t_construct<attr_t>())
		(L"attribute", t_member<attr_t(t_token::*)() const, &t_token::f_attribute>())
	;
}

t_scoped t_type_of<xemmaix::nata::curses::t_token>::f_construct(t_stacked* a_stack, size_t a_n)
{
	return t_construct<attr_t>::t_bind<xemmaix::nata::curses::t_token>::f_do(this, a_stack, a_n);
}

}

namespace xemmaix::nata::curses
{

namespace
{

void f_main(const t_value& a_callable)
{
	::nata::curses::t_session session;
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

t_scoped f_size()
{
	return f_tuple(COLS, LINES);
}

}

t_extension::t_extension(xemmai::t_object* a_module, t_scoped&& a_nata) : xemmai::t_extension(a_module), v_module_nata(std::move(a_nata))
{
	v_nata = xemmai::f_extension<xemmaix::nata::t_extension>(v_module_nata);
	t_type_of<t_token>::f_define(this);
	t_type_of<t_view<t_target>>::f_define(this);
	t_type_of<t_overlay<t_target>>::f_define(this);
	t_type_of<t_overlay_iterator<t_target>>::f_define(this);
	t_type_of<t_painter<t_target>>::f_define(this);
	t_type_of<t_creaser<t_target>>::f_define(this);
	f_define<void(*)(const t_value&), f_main>(this, L"main");
	f_define<void(*)(short, short, short), f_define_pair>(this, L"define_pair");
	f_define<attr_t(*)(short), f_color_pair>(this, L"color_pair");
	f_define<t_scoped(*)(), f_size>(this, L"size");
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
	a_scan(v_module_nata);
	a_scan(v_type_token);
	a_scan(v_type_view);
	a_scan(v_type_overlay);
	a_scan(v_type_overlay_iterator);
	a_scan(v_type_painter);
	a_scan(v_type_creaser);
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_object* a_module)
{
	return new xemmaix::nata::curses::t_extension(a_module, xemmai::t_module::f_instantiate(L"nata"));
}
