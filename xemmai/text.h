#ifndef XEMMAIX__NATA__TEXT_H
#define XEMMAIX__NATA__TEXT_H

#include <nata/model.h>
#include "proxy.h"

namespace xemmaix::nata
{

struct t_text : t_proxy
{
	::nata::t_text<>* v_text = new ::nata::t_text<>();

	static t_pvalue f_construct(t_type* a_class)
	{
		return a_class->f_new<t_text>();
	}

	virtual void f_destroy();
	size_t f_size() const
	{
		return v_text->f_size();
	}
	std::wstring f_slice(size_t a_p, size_t a_n) const
	{
		size_t n = f_size();
		if (a_p > n) f_throw(L"out of range."sv);
		return {v_text->f_at(a_p), v_text->f_at(a_p + std::min(a_n, n - a_p))};
	}
	void f_replace(size_t a_p, size_t a_n, std::wstring_view a_text)
	{
		size_t n = f_size();
		if (a_p > n) f_throw(L"out of range."sv);
		v_text->f_replace(a_p, std::min(a_n, n - a_p), a_text.begin(), a_text.end());
	}
	size_t f_lines() const
	{
		return v_text->f_lines().f_size().v_i0;
	}
	t_pvalue f_line_at_in_text(size_t a_p) const
	{
		size_t n = f_size();
		if (a_p > n) f_throw(L"out of range."sv);
		auto line = v_text->f_lines().f_at_in_text(a_p);
		return f_tuple(line.f_index().v_i0, line.f_index().v_i1, line.f_delta().v_i0, line.f_delta().v_i1);
	}
};

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::nata::t_text> : t_derivable<t_bears<xemmaix::nata::t_text, t_type_of<xemmaix::nata::t_proxy>>>
{
	static void f_define(t_library* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif
