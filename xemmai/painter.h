#ifndef XEMMAIX__NATA__PAINTER_H
#define XEMMAIX__NATA__PAINTER_H

#include <nata/painter.h>
#include "view.h"

namespace xemmaix::nata
{

template<typename T_target>
struct t_painter : t_proxy
{
	t_view<T_target>& v_view;
	::nata::t_painter<std::decay_t<decltype(*t_view<T_target>::v_tokens)>>* v_painter;

	::nata::t_text_replaced v_replaced = [this](auto, auto, auto)
	{
		f_reset();
	};
	::nata::t_connection<decltype(v_replaced)>* v_connection;

	static t_pvalue f_construct(t_type* a_class, t_view<T_target>& a_view)
	{
		return a_class->f_new<t_painter>(a_view);
	}

	t_painter(t_view<T_target>& a_view) : v_view(a_view), v_painter(new std::decay_t<decltype(*v_painter)>(*v_view.v_tokens)), v_connection(v_view.v_text.v_text->v_replaced >> v_replaced)
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
	void f_push(const t_pvalue& a_x, size_t a_n, size_t a_merge)
	{
		v_painter->f_push(a_x, std::min(a_n, v_view.v_text.v_text->f_size() - f_current()), a_merge);
	}
	void f_push(const t_pvalue& a_x, size_t a_n)
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
	using t_library = typename T_target::t_library;
	using t_painter = xemmaix::nata::t_painter<T_target>;

	static void f_define(t_library* a_library)
	{
		t_define{a_library}
			(L"reset"sv, t_member<void(t_painter::*)(), &t_painter::f_reset>())
			(L"current"sv, t_member<size_t(t_painter::*)() const, &t_painter::f_current>())
			(L"push"sv,
				t_member<void(t_painter::*)(const t_pvalue&, size_t), &t_painter::f_push>(),
				t_member<void(t_painter::*)(const t_pvalue&, size_t, size_t), &t_painter::f_push>()
			)
			(L"flush"sv, t_member<void(t_painter::*)(), &t_painter::f_flush>())
		.template f_derive<t_painter, xemmaix::nata::t_proxy>();
	}

	using t_type_of::t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n)
	{
		return t_construct_with<t_pvalue(*)(t_type*, xemmaix::nata::t_view<T_target>&), t_painter::f_construct>::template t_bind<t_painter>::f_do(this, a_stack, a_n);
	}
};

}

#endif
