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
	t_slot_of<t_type> v_type_token;
	t_slot_of<t_type> v_type_position;
	t_slot_of<t_type> v_type_row;
	t_slot_of<t_type> v_type_view;
	t_slot_of<t_type> v_type_overlay;
	t_slot_of<t_type> v_type_overlay_value;
	t_slot_of<t_type> v_type_overlay_iterator;
	t_slot_of<t_type> v_type_painter;
	t_slot_of<t_type> v_type_creaser;

public:
	t_library(xemmai::t_library::t_handle* a_handle, const t_pvalue& a_nata) : xemmai::t_library(a_handle), v_module_nata(a_nata), v_nata(&v_module_nata->f_as<t_module>().v_body->f_as<xemmaix::nata::t_library>())
	{
	}
	virtual void f_scan(t_scan a_scan);
	virtual std::vector<std::pair<t_root, t_rvalue>> f_define();
	template<typename T>
	const T* f_library() const
	{
		return f_global();
	}
	template<typename T>
	t_slot_of<t_type>& f_type_slot()
	{
		return v_nata->f_type_slot<T>();
	}
	template<typename T>
	t_type* f_type() const
	{
		return const_cast<t_library*>(this)->f_type_slot<T>();
	}
	template<typename T>
	t_pvalue f_as(T&& a_value) const
	{
		using t = t_type_of<typename t_fundamental<T>::t_type>;
		return t::f_transfer(f_library<typename t::t_library>(), std::forward<T>(a_value));
	}
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

template<>
inline const xemmaix::nata::t_library* t_library::f_library<xemmaix::nata::t_library>() const
{
	return v_nata;
}

template<>
inline const t_library* t_library::f_library<t_library>() const
{
	return this;
}

XEMMAI__LIBRARY__TYPE(t_library, token)
XEMMAI__LIBRARY__TYPE_AS(t_library, t_view<t_target>, view)
XEMMAI__LIBRARY__TYPE_AS(t_library, t_overlay<t_target>, overlay)
XEMMAI__LIBRARY__TYPE_AS(t_library, t_overlay_iterator<t_target>, overlay_iterator)
XEMMAI__LIBRARY__TYPE_AS(t_library, t_painter<t_target>, painter)
XEMMAI__LIBRARY__TYPE_AS(t_library, t_creaser<t_target>, creaser)

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
