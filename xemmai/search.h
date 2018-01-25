#ifndef XEMMAIX__NATA__SEARCH_H
#define XEMMAIX__NATA__SEARCH_H

#include "text.h"
#include <regex>
#include <xemmai/derived.h>

namespace xemmaix
{

namespace nata
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
		v_i = decltype(v_i)(v_p.f_begin(), v_p.f_end(), v_pattern);
	};
	::nata::t_connection<decltype(v_replaced)>* v_connection;

	static t_scoped f_construct(t_object* a_class, t_text& a_text)
	{
		return (new t_search(a_class, a_text))->f_object();
	}

	t_search(t_object* a_class, t_text& a_text) : t_proxy(a_class), v_text(a_text), v_p(*v_text.v_text), v_connection(v_p.v_replaced >> v_replaced)
	{
		v_text.f_acquire();
	}
	virtual void f_destroy();
	void f_pattern(const std::wstring& a_pattern, intptr_t a_flags)
	{
		v_pattern.assign(a_pattern, static_cast<std::wregex::flag_type>(a_flags));
		v_i = decltype(v_i)(v_p.f_begin(), v_p.f_end(), v_pattern);
	}
	t_scoped f_next();
};

}

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::nata::t_search> : t_type_of<xemmaix::nata::t_proxy>
{
	static void f_define(t_extension* a_extension);

	using t_type_of<xemmaix::nata::t_proxy>::t_type_of;
	virtual t_type* f_derive(t_object* a_this);
	virtual t_scoped f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n);
};

}

#endif
