#ifndef XEMMAIX__NATA__CREASER_H
#define XEMMAIX__NATA__CREASER_H

#include "../creaser.h"
#include "view.h"

namespace xemmaix::nata
{

template<typename T_target>
struct t_creaser : t_proxy
{
	t_view<T_target>& v_view;
	::nata::t_creaser<std::decay_t<decltype(*t_view<T_target>::v_rows)>, t_scoped>* v_creaser;

	::nata::t_slot<size_t, size_t, size_t> v_replaced = [this](auto, auto, auto)
	{
		f_reset();
	};
	::nata::t_connection<decltype(v_replaced)>* v_connection;

	static t_scoped f_construct(t_object* a_class, t_view<T_target>& a_view)
	{
		return (new t_creaser(a_class, a_view))->f_object();
	}

	t_creaser(t_object* a_class, t_view<T_target>& a_view) : t_proxy(a_class), v_view(a_view), v_creaser(new std::decay_t<decltype(*v_creaser)>(*v_view.v_rows)), v_connection(v_view.v_text.v_text->v_replaced >> v_replaced)
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
	void f_push(size_t a_n)
	{
		v_creaser->f_push(std::min(a_n, v_view.v_text.v_text->f_size() - f_current()));
	}
	t_scoped f_tag() const
	{
		return v_creaser->f_tag();
	}
	void f_open(t_scoped&& a_tag)
	{
		v_creaser->f_open(std::move(a_tag));
	}
	void f_close()
	{
		v_creaser->f_close();
	}
	void f_flush()
	{
		v_creaser->f_flush();
	}
};

}

namespace xemmai
{

template<typename T_target>
struct t_type_of<xemmaix::nata::t_creaser<T_target>> : t_type_of<xemmaix::nata::t_proxy>
{
	typedef typename T_target::t_extension t_extension;
	using t_creaser = xemmaix::nata::t_creaser<T_target>;

	static void f_define(t_extension* a_extension)
	{
		t_define<t_creaser, xemmaix::nata::t_proxy>(a_extension, L"Creaser")
			(t_construct_with<t_scoped(*)(t_object*, xemmaix::nata::t_view<T_target>&), t_creaser::f_construct>())
			(L"reset", t_member<void(t_creaser::*)(), &t_creaser::f_reset>())
			(L"current", t_member<size_t(t_creaser::*)() const, &t_creaser::f_current>())
			(L"push", t_member<void(t_creaser::*)(size_t), &t_creaser::f_push>())
			(L"tag", t_member<t_scoped(t_creaser::*)() const, &t_creaser::f_tag>())
			(L"open", t_member<void(t_creaser::*)(t_scoped&&), &t_creaser::f_open>())
			(L"close", t_member<void(t_creaser::*)(), &t_creaser::f_close>())
			(L"flush", t_member<void(t_creaser::*)(), &t_creaser::f_flush>())
		;
	}

	using t_type_of<xemmaix::nata::t_proxy>::t_type_of;
	virtual t_type* f_derive(t_object* a_this)
	{
		return new t_derived<t_type_of>(t_scoped(v_module), a_this);
	}
	virtual t_scoped f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n)
	{
		return t_construct_with<t_scoped(*)(t_object*, xemmaix::nata::t_view<T_target>&), t_creaser::f_construct>::template t_bind<t_creaser>::f_do(a_class, a_stack, a_n);
	}
};

}

#endif
