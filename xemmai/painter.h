#ifndef XEMMAIX__NATA__PAINTER_H
#define XEMMAIX__NATA__PAINTER_H

#include <nata/painter.h>
#include "view.h"

namespace xemmaix::nata
{

template<typename T_target>
struct t_painter : t_proxy, ::nata::t_painter<decltype(t_view<T_target>::v_tokens)>
{
	t_view<T_target>& v_view;

	::nata::t_text_replaced v_replaced = [this](auto, auto, auto)
	{
		this->f_reset();
	};
	::nata::t_connection<decltype(v_replaced)>* v_connection;

	t_painter(t_view<T_target>& a_view) : ::nata::t_painter<decltype(t_view<T_target>::v_tokens)>(a_view.v_tokens), v_view(a_view), v_connection(v_view.v_text.v_replaced >> v_replaced)
	{
		v_view.f_acquire();
	}
	virtual void f_destroy()
	{
		delete v_connection;
		this->::nata::t_painter<decltype(t_view<T_target>::v_tokens)>::~t_painter();
		v_view.f_release();
	}
	void f_push(const t_pvalue& a_x, size_t a_n, size_t a_merge)
	{
		::nata::t_painter<decltype(t_view<T_target>::v_tokens)>::f_push(a_x, std::min(a_n, v_view.v_text.f_size() - this->f_current()), a_merge);
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
		(L"reset"sv, t_member<void(*)(t_painter&), [](auto a_this)
		{
			a_this.f_reset();
		}>())
		(L"current"sv, t_member<size_t(*)(const t_painter&), [](auto a_this)
		{
			return a_this.f_current();
		}>())
		(L"push"sv,
			t_member<void(*)(t_painter&, const t_pvalue&, size_t), [](auto a_this, auto a_x, auto a_n)
			{
				a_this.f_push(a_x, a_n, 0);
			}>(),
			t_member<void(t_painter::*)(const t_pvalue&, size_t, size_t), &t_painter::f_push>()
		)
		(L"flush"sv, t_member<void(*)(t_painter&), [](auto a_this)
		{
			a_this.f_flush();
		}>())
		.template f_derive<t_painter, xemmaix::nata::t_proxy>();
	}

	using t_type_of::t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n)
	{
		return t_construct<xemmaix::nata::t_view<T_target>&>::template t_bind<t_painter>::f_do(this, a_stack, a_n);
	}
};

}

#endif
