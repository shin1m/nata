#ifndef XEMMAIX__NATA__SEARCH_H
#define XEMMAIX__NATA__SEARCH_H

#include <regex>
#include "text.h"

namespace xemmaix::nata
{

t_object* f_result(t_library* a_library, const std::match_results<decltype(t_text().f_begin())>& a_match);

struct t_pattern : t_proxy
{
	std::wregex v_pattern;

	t_pattern(std::wstring_view a_pattern, intptr_t a_flags) : v_pattern(a_pattern.begin(), a_pattern.end(), static_cast<std::wregex::flag_type>(a_flags))
	{
	}
	virtual void f_destroy();
};

struct t_search : t_proxy
{
	t_text& v_text;
	t_pattern& v_pattern;
	std::regex_iterator<decltype(v_text.f_begin())> v_eos;
	std::regex_iterator<decltype(v_text.f_begin())> v_i;

	::nata::t_text_replaced v_replaced = [this](auto, auto, auto)
	{
		v_i = v_eos;
	};
	::nata::t_connection<decltype(v_replaced)>* v_connection;

	t_search(t_text& a_text, t_pattern& a_pattern) : v_text(a_text), v_pattern(a_pattern), v_connection(v_text.v_replaced >> v_replaced)
	{
		v_text.f_acquire();
		v_pattern.f_acquire();
	}
	virtual void f_destroy();
	t_object* f_get(t_library* a_library) const;
	t_object* f_first(t_library* a_library, size_t a_p, size_t a_n)
	{
		size_t n = v_text.f_size();
		if (a_p > n) f_throw(L"out of range."sv);
		v_i = {v_text.f_at(a_p), v_text.f_at(a_p + std::min(a_n, n - a_p)), v_pattern.v_pattern};
		return f_result(a_library, *v_i);
	}
	t_object* f_next(t_library* a_library)
	{
		if (v_i != v_eos) ++v_i;
		return f_result(a_library, *v_i);
	}
};

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::nata::t_pattern> : t_derivable<t_bears<xemmaix::nata::t_pattern, t_type_of<xemmaix::nata::t_proxy>>>
{
	static void f_define(t_library* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

template<>
struct t_type_of<xemmaix::nata::t_search> : t_derivable<t_bears<xemmaix::nata::t_search, t_type_of<xemmaix::nata::t_proxy>>>
{
	static void f_define(t_library* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif
