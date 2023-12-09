#include "curses.h"

namespace xemmai
{

void t_type_of<xemmaix::nata::curses::t_token>::f_define(t_library* a_library)
{
	using xemmaix::nata::curses::t_token;
	t_define{a_library}
		(L"attribute"sv, t_member<attr_t(t_token::*)() const, &t_token::f_attribute>())
	.f_derive<t_token, t_object>();
}

t_pvalue t_type_of<xemmaix::nata::curses::t_token>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct<attr_t>::t_bind<xemmaix::nata::curses::t_token>::f_do(this, a_stack, a_n);
}

}

namespace xemmaix::nata::curses
{

namespace
{

void f_main(const t_pvalue& a_callable)
{
	::nata::curses::t_session session;
	a_callable();
}

void f_define_pair(short a_pair, short a_fore, short a_back)
{
	if (init_pair(a_pair, a_fore, a_back) == ERR) f_throw(L"init_pair"sv);
}

attr_t f_color_pair(short a_pair)
{
	return COLOR_PAIR(a_pair);
}

t_pvalue f_size()
{
	return f_tuple(COLS, LINES);
}

void f_flush()
{
	if (doupdate() == ERR) f_throw(L"doupdate"sv);
}

int v_resized[2];
struct sigaction v_sigwinch;

struct t_resized
{
	t_resized()
	{
		if (pipe(v_resized) != 0) portable::f_throw_system_error();
		struct sigaction sa;
		sa.sa_handler = [](int a_signal)
		{
			v_sigwinch.sa_handler(a_signal);
			auto e = errno;
			for (char b; write(v_resized[1], &b, 1) == -1;) if (errno != EINTR) std::exit(errno);
			errno = e;
		};
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = 0;
		if (sigaction(SIGWINCH, &sa, &v_sigwinch) == -1) portable::f_throw_system_error();
	}
	~t_resized() noexcept(false)
	{
		if (sigaction(SIGWINCH, &v_sigwinch, NULL) == -1) portable::f_throw_system_error();
		for (auto x : v_resized) while (close(x) == -1) if (errno != EINTR) portable::f_throw_system_error();
	}
};

void f_main_with_resized(const t_pvalue& a_callable)
{
	::nata::curses::t_session session;
	t_resized resized;
	a_callable(v_resized[0]);
}

void f_read_resized()
{
	for (char b; read(v_resized[0], &b, 1) == -1;) if (errno != EINTR) portable::f_throw_system_error();
	if (doupdate() == ERR) f_throw(L"doupdate"sv);
}

}

void t_library::f_scan(t_scan a_scan)
{
	a_scan(v_module_nata);
	a_scan(v_type_token);
	a_scan(v_type_position);
	a_scan(v_type_row);
	a_scan(v_type_view);
	a_scan(v_type_overlay);
	a_scan(v_type_overlay_value);
	a_scan(v_type_overlay_iterator);
	a_scan(v_type_painter);
	a_scan(v_type_creaser);
}

std::vector<std::pair<t_root, t_rvalue>> t_library::f_define()
{
	t_type_of<t_token>::f_define(this);
	v_type_position.f_construct(f_global()->f_type<t_object>()->f_derive({{
		t_symbol::f_instantiate(L"text"sv),
		t_symbol::f_instantiate(L"x"sv),
		t_symbol::f_instantiate(L"width"sv)
	}}));
	v_type_row.f_construct(f_global()->f_type<t_object>()->f_derive({{
		t_symbol::f_instantiate(L"index"sv),
		t_symbol::f_instantiate(L"line"sv),
		t_symbol::f_instantiate(L"text"sv),
		t_symbol::f_instantiate(L"x"sv),
		t_symbol::f_instantiate(L"y"sv)
	}}));
	v_type_overlay_value.f_construct(f_global()->f_type<t_object>()->f_derive({{
		t_symbol::f_instantiate(L"value"sv),
		t_symbol::f_instantiate(L"from"sv),
		t_symbol::f_instantiate(L"count"sv)
	}}));
	t_type_of<t_view<t_target>>::f_define(this);
	t_type_of<t_overlay<t_target>>::f_define(this);
	t_type_of<t_overlay_iterator<t_target>>::f_define(this);
	t_type_of<t_painter<t_target>>::f_define(this);
	t_type_of<t_creaser<t_target>>::f_define(this);
	return t_define(this)
		(L"Token"sv, static_cast<t_object*>(v_type_token))
		(L"Position"sv, static_cast<t_object*>(v_type_position))
		(L"Row"sv, static_cast<t_object*>(v_type_row))
		(L"View"sv, static_cast<t_object*>(v_type_view))
		(L"Overlay"sv, static_cast<t_object*>(v_type_overlay))
		(L"OverlayValue"sv, static_cast<t_object*>(v_type_overlay_value))
		(L"OverlayIterator"sv, static_cast<t_object*>(v_type_overlay_iterator))
		(L"Painter"sv, static_cast<t_object*>(v_type_painter))
		(L"Creaser"sv, static_cast<t_object*>(v_type_creaser))
		(L"main"sv, t_static<void(*)(const t_pvalue&), f_main>())
		(L"define_pair"sv, t_static<void(*)(short, short, short), f_define_pair>())
		(L"color_pair"sv, t_static<attr_t(*)(short), f_color_pair>())
		(L"size"sv, t_static<t_pvalue(*)(), f_size>())
		(L"flush"sv, t_static<void(*)(), f_flush>())
		(L"main_with_resized"sv, t_static<void(*)(const t_pvalue&), f_main_with_resized>())
		(L"read_resized"sv, t_static<void(*)(), f_read_resized>())
		(L"COLOR_BLACK"sv, COLOR_BLACK)
		(L"COLOR_RED"sv, COLOR_RED)
		(L"COLOR_GREEN"sv, COLOR_GREEN)
		(L"COLOR_YELLOW"sv, COLOR_YELLOW)
		(L"COLOR_BLUE"sv, COLOR_BLUE)
		(L"COLOR_MAGENTA"sv, COLOR_MAGENTA)
		(L"COLOR_CYAN"sv, COLOR_CYAN)
		(L"COLOR_WHITE"sv, COLOR_WHITE)
		(L"A_NORMAL"sv, A_NORMAL)
		(L"A_STANDOUT"sv, A_STANDOUT)
		(L"A_UNDERLINE"sv, A_UNDERLINE)
		(L"A_REVERSE"sv, A_REVERSE)
		(L"A_BLINK"sv, A_BLINK)
		(L"A_DIM"sv, A_DIM)
		(L"A_BOLD"sv, A_BOLD)
		(L"A_ALTCHARSET"sv, A_ALTCHARSET)
		(L"A_INVIS"sv, A_INVIS)
		(L"A_PROTECT"sv, A_PROTECT)
		(L"A_HORIZONTAL"sv, A_HORIZONTAL)
		(L"A_LEFT"sv, A_LEFT)
		(L"A_LOW"sv, A_LOW)
		(L"A_RIGHT"sv, A_RIGHT)
		(L"A_TOP"sv, A_TOP)
		(L"A_VERTICAL"sv, A_VERTICAL)
		(L"KEY_RESIZE"sv, KEY_RESIZE)
		(L"KEY_DOWN"sv, KEY_DOWN)
		(L"KEY_UP"sv, KEY_UP)
		(L"KEY_LEFT"sv, KEY_LEFT)
		(L"KEY_RIGHT"sv, KEY_RIGHT)
		(L"KEY_BACKSPACE"sv, KEY_BACKSPACE)
		(L"KEY_F1"sv, KEY_F(1))
		(L"KEY_F2"sv, KEY_F(2))
		(L"KEY_F3"sv, KEY_F(3))
		(L"KEY_F4"sv, KEY_F(4))
		(L"KEY_F5"sv, KEY_F(5))
		(L"KEY_F6"sv, KEY_F(6))
		(L"KEY_F7"sv, KEY_F(7))
		(L"KEY_F8"sv, KEY_F(8))
		(L"KEY_F9"sv, KEY_F(9))
		(L"KEY_F10"sv, KEY_F(10))
		(L"KEY_F11"sv, KEY_F(11))
		(L"KEY_F12"sv, KEY_F(12))
		(L"KEY_ENTER"sv, KEY_ENTER)
	;
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_library::t_handle* a_handle)
{
	using namespace std::literals;
	return xemmai::f_new<xemmaix::nata::curses::t_library>(a_handle, xemmai::t_module::f_instantiate(L"nata"sv));
}
