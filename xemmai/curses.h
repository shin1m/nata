#ifndef XEMMAIX__NATA__CURSES_H
#define XEMMAIX__NATA__CURSES_H

#include <nata/curses.h>
#include "painter.h"
#include "creaser.h"

namespace xemmaix::nata::curses
{

struct t_token
{
	attr_t v_attribute;

	t_token(attr_t a_attribute) : v_attribute(a_attribute)
	{
	}
	attr_t f_attribute() const
	{
		return v_attribute;
	}
};

class t_library : public xemmai::t_library
{
	t_slot v_module_nata;
	xemmaix::nata::t_library* v_nata;
#define XEMMAIX__NATA__CURSES__TYPES(_)\
	_(token)\
	_##_JUST(position)\
	_##_JUST(row)\
	_##_AS(t_view<t_target>, view)\
	_##_AS(t_overlay<t_target>, overlay)\
	_##_JUST(overlay_value)\
	_##_AS(t_overlay_iterator<t_target>, overlay_iterator)\
	_##_AS(t_painter<t_target>, painter)\
	_##_AS(t_creaser<t_target>, creaser)
	XEMMAIX__NATA__CURSES__TYPES(XEMMAI__TYPE__DECLARE)

public:
	t_library(xemmai::t_library::t_handle* a_handle, const t_pvalue& a_nata) : xemmai::t_library(a_handle), v_module_nata(a_nata), v_nata(&v_module_nata->f_as<t_module>().v_body->f_as<xemmaix::nata::t_library>())
	{
	}
	XEMMAI__LIBRARY__MEMBERS
	t_type* f_type_position() const
	{
		return v_type_position;
	}
	t_type* f_type_row() const
	{
		return v_type_row;
	}
	t_type* f_type_overlay_value() const
	{
		return v_type_overlay_value;
	}
};

struct t_target : ::nata::curses::t_target
{
	using t_library = xemmaix::nata::curses::t_library;

	using ::nata::curses::t_target::t_target;
	std::tuple<size_t, size_t, size_t> f_size(wchar_t a_c, const t_rvalue& a_a) const
	{
		return ::nata::curses::t_target::f_size(a_c, {});
	}
	std::tuple<size_t, size_t, size_t> f_tab(size_t a_x, const t_rvalue& a_a) const
	{
		return ::nata::curses::t_target::f_tab(a_x, {});
	}
	std::tuple<size_t, size_t, size_t> f_eol(const t_rvalue& a_a) const
	{
		return ::nata::curses::t_target::f_eol({});
	}

	struct t_graphics : ::nata::curses::t_graphics
	{
		using ::nata::curses::t_graphics::t_graphics;
		void f_attribute(const t_rvalue& a_value);
	};
};

XEMMAI__LIBRARY__BASE(t_library, t_global, f_global())
XEMMAI__LIBRARY__BASE(t_library, xemmaix::nata::t_library, v_nata)
#define XEMMAI__TYPE__LIBRARY t_library
XEMMAIX__NATA__CURSES__TYPES(XEMMAI__TYPE__DEFINE)
#undef XEMMAI__TYPE__LIBRARY

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::nata::curses::t_token> : t_derivable<t_holds<xemmaix::nata::curses::t_token>>
{
	using t_library = xemmaix::nata::curses::t_library;

	static void f_define(t_library* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

namespace xemmaix::nata::curses
{

inline void t_target::t_graphics::f_attribute(const t_rvalue& a_value)
{
	::nata::curses::t_graphics::f_attribute(f_is<t_token>(a_value) ? f_as<t_token&>(a_value).v_attribute : t_attribute{});
}

}

#endif
