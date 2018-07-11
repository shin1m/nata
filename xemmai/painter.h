#ifndef XEMMAIX__NATA__PAINTER_H
#define XEMMAIX__NATA__PAINTER_H

#include "../painter.h"
#include "view.h"

namespace xemmaix::nata
{

template<typename T_target>
struct t_painter : t_proxy
{
	t_view<T_target>& v_view;
	::nata::t_painter<std::decay_t<decltype(*t_view<T_target>::v_tokens)>>* v_painter;

	::nata::t_slot<size_t, size_t, size_t> v_replaced = [this](auto, auto, auto)
	{
		f_reset();
	};
	::nata::t_connection<decltype(v_replaced)>* v_connection;

	static t_scoped f_construct(t_type* a_class, t_view<T_target>& a_view)
	{
		return (new t_painter(a_class, a_view))->f_object();
	}

	t_painter(t_type* a_class, t_view<T_target>& a_view) : t_proxy(a_class), v_view(a_view), v_painter(new std::decay_t<decltype(*v_painter)>(*v_view.v_tokens)), v_connection(v_view.v_text.v_text->v_replaced >> v_replaced)
	{
		v_view.f_acquire();
	}
	virtual void f_destroy()
	{
		delete v_connection;
		delete v_painter;
		v_view.f_release();
	}
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

namespace xemmai
{

template<typename T_target>
struct t_type_of<xemmaix::nata::t_painter<T_target>> : t_derivable<t_bears<xemmaix::nata::t_painter<T_target>, t_type_of<xemmaix::nata::t_proxy>>>
{
	typedef typename T_target::t_extension t_extension;
	using t_painter = xemmaix::nata::t_painter<T_target>;

	static void f_define(t_extension* a_extension)
	{
		t_define<t_painter, xemmaix::nata::t_proxy>(a_extension, L"Painter")
			(t_construct_with<t_scoped(*)(t_type*, xemmaix::nata::t_view<T_target>&), t_painter::f_construct>())
			(L"reset", t_member<void(t_painter::*)(), &t_painter::f_reset>())
			(L"current", t_member<size_t(t_painter::*)() const, &t_painter::f_current>())
			(L"push",
				t_member<void(t_painter::*)(const t_value&, size_t), &t_painter::f_push>(),
				t_member<void(t_painter::*)(const t_value&, size_t, size_t), &t_painter::f_push>()
			)
			(L"flush", t_member<void(t_painter::*)(), &t_painter::f_flush>())
		;
	}

	using t_type_of::t_base::t_base;
	virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n)
	{
		return t_construct_with<t_scoped(*)(t_type*, xemmaix::nata::t_view<T_target>&), t_painter::f_construct>::template t_bind<t_painter>::f_do(this, a_stack, a_n);
	}
};

}

#endif
