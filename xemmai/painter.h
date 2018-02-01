#ifndef XEMMAIX__NATA__PAINTER_H
#define XEMMAIX__NATA__PAINTER_H

#include "../painter.h"
#include "view.h"

namespace xemmaix
{

namespace nata
{

struct t_painter : t_proxy
{
	typedef decltype(std::decay_t<decltype(*t_view::v_tokens)>::t_span::v_x) t_value;

	t_view& v_view;
	::nata::t_painter<std::decay_t<decltype(*t_view::v_tokens)>>* v_painter;

	::nata::t_slot<size_t, size_t, size_t> v_replaced = [this](auto, auto, auto)
	{
		f_reset();
	};
	::nata::t_connection<decltype(v_replaced)>* v_connection;

	static t_scoped f_construct(t_object* a_class, t_view& a_view)
	{
		return (new t_painter(a_class, a_view))->f_object();
	}

	t_painter(t_object* a_class, t_view& a_view) : t_proxy(a_class), v_view(a_view), v_painter(new std::decay_t<decltype(*v_painter)>(*v_view.v_tokens)), v_connection(v_view.v_text.v_text->v_replaced >> v_replaced)
	{
		v_view.f_acquire();
	}
	virtual void f_destroy();
	void f_reset()
	{
		v_painter->f_reset();
	}
	size_t f_current() const
	{
		return v_painter->f_current();
	}
	void f_push(const t_value& a_x, size_t a_n, size_t a_merge)
	{
		v_painter->f_push(a_x, std::min(a_n, v_view.v_text.v_text->f_size() - f_current()), a_merge);
	}
	void f_push(const t_value& a_x, size_t a_n)
	{
		f_push(a_x, a_n, 0);
	}
	void f_flush()
	{
		v_painter->f_flush();
	}
};

}

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::nata::t_painter> : t_type_of<xemmaix::nata::t_proxy>
{
	static void f_define(t_extension* a_extension);

	using t_type_of<xemmaix::nata::t_proxy>::t_type_of;
	virtual t_type* f_derive(t_object* a_this);
	virtual t_scoped f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n);
};

}

#endif
