#ifndef XEMMAIX__NATA__CREASER_H
#define XEMMAIX__NATA__CREASER_H

#include <nata/creaser.h>
#include "view.h"

namespace xemmaix::nata
{

template<typename T_target>
struct t_creaser : t_proxy
{
	t_view<T_target>& v_view;
	::nata::t_creaser<std::decay_t<decltype(*t_view<T_target>::v_rows)>>* v_creaser;

	::nata::t_text_replaced v_replaced = [this](auto, auto, auto)
	{
		f_reset();
	};
	::nata::t_connection<decltype(v_replaced)>* v_connection;

	static t_pvalue f_construct(t_type* a_class, t_view<T_target>& a_view)
	{
		return a_class->f_new<t_creaser>(a_view);
	}

	t_creaser(t_view<T_target>& a_view) : v_view(a_view), v_creaser(new std::decay_t<decltype(*v_creaser)>(*v_view.v_rows)), v_connection(v_view.v_text.v_text->v_replaced >> v_replaced)
	{
		v_view.f_acquire();
	}
	virtual void f_destroy()
	{
		delete v_connection;
		delete v_creaser;
		v_view.f_release();
	}
	void f_reset()
	{
		v_creaser->f_reset();
	}
	size_t f_current() const
	{
		return v_creaser->f_current();
	}
	void f_push(size_t a_plain, size_t a_nested)
	{
		if (a_nested <= 0) f_throw(L"nested must be greater than zero."sv);
		size_t n = v_view.v_text.v_text->f_size() - f_current();
		if (a_plain > n || a_nested > n || a_plain + a_nested > n) f_throw(L"out of range."sv);
		v_creaser->f_push(a_plain, a_nested);
	}
	void f_end()
	{
		v_creaser->f_end();
	}
};

}

namespace xemmai
{

template<typename T_target>
struct t_type_of<xemmaix::nata::t_creaser<T_target>> : t_derivable<t_bears<xemmaix::nata::t_creaser<T_target>, t_type_of<xemmaix::nata::t_proxy>>>
{
	using t_library = typename T_target::t_library;
	using t_creaser = xemmaix::nata::t_creaser<T_target>;

	static void f_define(t_library* a_library)
	{
		t_define{a_library}
			(L"reset"sv, t_member<void(t_creaser::*)(), &t_creaser::f_reset>())
			(L"current"sv, t_member<size_t(t_creaser::*)() const, &t_creaser::f_current>())
			(L"push"sv, t_member<void(t_creaser::*)(size_t, size_t), &t_creaser::f_push>())
			(L"end"sv, t_member<void(t_creaser::*)(), &t_creaser::f_end>())
		.template f_derive<t_creaser, xemmaix::nata::t_proxy>();
	}

	using t_type_of::t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n)
	{
		return t_construct_with<t_pvalue(*)(t_type*, xemmaix::nata::t_view<T_target>&), t_creaser::f_construct>::template t_bind<t_creaser>::f_do(this, a_stack, a_n);
	}
};

}

#endif
