#ifndef XEMMAIX__NATA__TEXT_H
#define XEMMAIX__NATA__TEXT_H

#include "../model.h"
#include "proxy.h"
#include <xemmai/derived.h>
#include <xemmai/tuple.h>

namespace xemmaix
{

namespace nata
{

struct t_text : t_proxy
{
	::nata::t_text<>* v_text = new ::nata::t_text<>();

	static t_scoped f_construct(t_object* a_class)
	{
		return (new t_text(a_class))->f_object();
	}

	using t_proxy::t_proxy;
	virtual void f_destroy();
	size_t f_size() const
	{
		return v_text->f_size();
	}
	std::wstring f_slice(size_t a_p, size_t a_n) const
	{
		size_t n = f_size();
		if (a_p > n) t_throwable::f_throw(L"out of range.");
		return {v_text->f_at(a_p), v_text->f_at(a_p + std::min(a_n, n - a_p))};
	}
	void f_replace(size_t a_p, size_t a_n, const std::wstring& a_text)
	{
		size_t n = f_size();
		if (a_p > n) t_throwable::f_throw(L"out of range.");
		v_text->f_replace(a_p, std::min(a_n, n - a_p), a_text.begin(), a_text.end());
	}
	size_t f_lines() const
	{
		return v_text->f_lines().f_size().v_i0;
	}
	t_scoped f_line_at_in_text(size_t a_p) const
	{
		size_t n = f_size();
		if (a_p > n) t_throwable::f_throw(L"out of range.");
		auto line = v_text->f_lines().f_at_in_text(a_p);
		t_scoped p = t_tuple::f_instantiate(4);
		auto& tuple = f_as<t_tuple&>(p);
		tuple[0].f_construct(t_value(line.f_index().v_i0));
		tuple[1].f_construct(t_value(line.f_index().v_i1));
		tuple[2].f_construct(t_value(line.f_delta().v_i0));
		tuple[3].f_construct(t_value(line.f_delta().v_i1));
		return p;
	}
};

}

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::nata::t_text> : t_type_of<xemmaix::nata::t_proxy>
{
	static void f_define(t_extension* a_extension);

	using t_type_of<xemmaix::nata::t_proxy>::t_type_of;
	virtual t_type* f_derive(t_object* a_this);
	virtual t_scoped f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n);
};

}

#endif
