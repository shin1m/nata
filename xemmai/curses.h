#ifndef XEMMAIX__NATA__CURSES_H
#define XEMMAIX__NATA__CURSES_H

#include "../curses.h"
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

class t_extension : public xemmai::t_extension
{
	t_slot v_module_nata;
	xemmaix::nata::t_extension* v_nata;
	t_slot_of<t_type> v_type_token;
	t_slot_of<t_type> v_type_view;
	t_slot_of<t_type> v_type_overlay;
	t_slot_of<t_type> v_type_overlay_iterator;
	t_slot_of<t_type> v_type_painter;
	t_slot_of<t_type> v_type_creaser;

public:
	t_extension(xemmai::t_object* a_module, t_scoped&& a_nata);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	const T* f_extension() const
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
		return const_cast<t_extension*>(this)->f_type_slot<T>();
	}
	template<typename T>
	t_scoped f_as(T&& a_value) const
	{
		typedef t_type_of<typename t_fundamental<T>::t_type> t;
		return t::f_transfer(f_extension<typename t::t_extension>(), std::forward<T>(a_value));
	}
};

struct t_target : ::nata::curses::t_target
{
	typedef xemmaix::nata::curses::t_extension t_extension;

	using ::nata::curses::t_target::t_target;
	std::tuple<size_t, size_t, size_t> f_size(wchar_t a_c, const t_scoped& a_a) const
	{
		return ::nata::curses::t_target::f_size(a_c, {});
	}
	std::tuple<size_t, size_t, size_t> f_tab(size_t a_x, const t_scoped& a_a) const
	{
		return ::nata::curses::t_target::f_tab(a_x, {});
	}
	std::tuple<size_t, size_t, size_t> f_eol(const t_scoped& a_a) const
	{
		return ::nata::curses::t_target::f_eol({});
	}

	struct t_graphics : ::nata::curses::t_graphics
	{
		using ::nata::curses::t_graphics::t_graphics;
		void f_attribute(const t_scoped& a_value);
	};
};

template<>
inline const xemmaix::nata::t_extension* t_extension::f_extension<xemmaix::nata::t_extension>() const
{
	return v_nata;
}

template<>
inline const t_extension* t_extension::f_extension<t_extension>() const
{
	return this;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_token>()
{
	return v_type_token;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_view<t_target>>()
{
	return v_type_view;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_overlay<t_target>>()
{
	return v_type_overlay;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_overlay_iterator<t_target>>()
{
	return v_type_overlay_iterator;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_painter<t_target>>()
{
	return v_type_painter;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_creaser<t_target>>()
{
	return v_type_creaser;
}

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::nata::curses::t_token> : t_derivable<t_holds<xemmaix::nata::curses::t_token>>
{
	typedef xemmaix::nata::curses::t_extension t_extension;

	static void f_define(t_extension* a_extension);

	using t_base::t_base;
	t_scoped f_do_construct(t_stacked* a_stack, size_t a_n);
};

}

namespace xemmaix::nata::curses
{

inline void t_target::t_graphics::f_attribute(const t_scoped& a_value)
{
	::nata::curses::t_graphics::f_attribute(f_is<t_token>(a_value) ? f_as<t_token&>(a_value).v_attribute : t_attribute{});
}

}

#endif
