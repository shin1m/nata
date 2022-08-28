#ifndef XEMMAIX__NATA__SEARCH_H
#define XEMMAIX__NATA__SEARCH_H

#include <regex>
#include "text.h"

namespace xemmaix::nata
{

struct t_search : t_proxy
{
	t_text& v_text;
	decltype(*v_text.v_text) v_p;
	std::wregex v_pattern;
	std::regex_iterator<decltype(v_p.f_begin()), wchar_t> v_eos;
	std::regex_iterator<decltype(v_p.f_begin()), wchar_t> v_i;

	::nata::t_slot<size_t, size_t, size_t> v_replaced = [this](auto, auto, auto)
	{
		f_reset(0, -1);
	};
	::nata::t_connection<decltype(v_replaced)>* v_connection;

	static t_pvalue f_construct(t_type* a_class, t_text& a_text)
	{
		return a_class->f_new<t_search>(a_text);
	}

	t_search(t_text& a_text) : v_text(a_text), v_p(*v_text.v_text), v_connection(v_p.v_replaced >> v_replaced)
	{
		v_text.f_acquire();
	}
	virtual void f_destroy();
	void f_pattern(std::wstring_view a_pattern, intptr_t a_flags)
	{
		v_pattern.assign(a_pattern.begin(), a_pattern.end(), static_cast<std::wregex::flag_type>(a_flags));
	}
	void f_reset(size_t a_p, size_t a_n)
	{
		size_t n = v_p.f_size();
		if (a_p > n) f_throw(L"out of range."sv);
		v_i = {v_p.f_at(a_p), v_p.f_at(a_p + std::min(a_n, n - a_p)), v_pattern};
	}
	t_pvalue f_next(t_library* a_library);
};

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::nata::t_search> : t_derivable<t_bears<xemmaix::nata::t_search, t_type_of<xemmaix::nata::t_proxy>>>
{
	static void f_define(t_library* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif
