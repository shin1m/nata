#ifndef XEMMAIX__NATA__CREASER_H
#define XEMMAIX__NATA__CREASER_H

#include <nata/creaser.h>
#include "view.h"

namespace xemmaix::nata
{

template<typename T_target>
struct t_creaser : t_proxy, ::nata::t_creaser<decltype(t_view<T_target>::v_rows)>
{
	t_view<T_target>& v_view;

	::nata::t_text_replaced v_replaced = [this](auto, auto, auto)
	{
		this->f_reset();
	};
	::nata::t_connection<decltype(v_replaced)>* v_connection;

	t_creaser(t_view<T_target>& a_view) : ::nata::t_creaser<decltype(t_view<T_target>::v_rows)>(a_view.v_rows), v_view(a_view), v_connection(v_view.v_text.v_replaced >> v_replaced)
	{
		v_view.f_acquire();
	}
	virtual void f_destroy()
	{
		delete v_connection;
		this->::nata::t_creaser<decltype(t_view<T_target>::v_rows)>::~t_creaser();
		v_view.f_release();
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
		(L"reset"sv, t_member<void(*)(t_creaser&), [](auto a_this)
		{
			a_this.f_reset();
		}>())
		(L"current"sv, t_member<size_t(*)(const t_creaser&), [](auto a_this)
		{
			return a_this.f_current();
		}>())
		(L"push"sv, t_member<void(*)(t_creaser&, size_t, size_t), [](auto a_this, size_t a_plain, size_t a_nested)
		{
			if (a_nested <= 0) f_throw(L"nested must be greater than zero."sv);
			size_t n = a_this.v_view.v_text.f_size() - a_this.f_current();
			if ((a_plain > n || a_nested > n || a_plain + a_nested > n)) f_throw(L"out of range."sv);
			a_this.f_push(a_plain, a_nested);
		}>())
		(L"end"sv, t_member<void(*)(t_creaser&), [](auto a_this)
		{
			a_this.f_end();
		}>())
		.template f_derive<t_creaser, xemmaix::nata::t_proxy>();
	}

	using t_type_of::t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n)
	{
		return t_construct<xemmaix::nata::t_view<T_target>&>::template t_bind<t_creaser>::f_do(this, a_stack, a_n);
	}
};

}

#endif
