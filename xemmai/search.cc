#include "search.h"

namespace xemmaix::nata
{

t_object* f_result(t_library* a_library, const std::match_results<decltype(t_text().f_begin())>& a_match)
{
	return t_tuple::f_instantiate(a_match.size(), [&](auto& tuple)
	{
		for (size_t i = 0; i < a_match.size(); ++i) {
			auto& m = a_match[i];
			size_t j = m.first.f_index();
			new(&tuple[i]) t_svalue(f_new_value(a_library->f_type_span(), j, m.second.f_index() - j));
		}
	});
}

void t_pattern::f_destroy()
{
	v_pattern.~basic_regex();
}

void t_search::f_destroy()
{
	delete v_connection;
	v_eos.~regex_iterator();
	v_i.~regex_iterator();
	v_pattern.f_release();
	v_text.f_release();
}

}

namespace xemmai
{

void t_type_of<xemmaix::nata::t_pattern>::f_define(t_library* a_library)
{
	using xemmaix::nata::t_pattern;
	t_define{a_library}
	(L"ICASE"sv, static_cast<intptr_t>(std::wregex::icase))
	(L"NOSUBS"sv, static_cast<intptr_t>(std::wregex::nosubs))
	(L"OPTIMIZE"sv, static_cast<intptr_t>(std::wregex::optimize))
	(L"COLLATE"sv, static_cast<intptr_t>(std::wregex::collate))
	(L"ECMASCRIPT"sv, static_cast<intptr_t>(std::wregex::ECMAScript))
	(L"BASIC"sv, static_cast<intptr_t>(std::wregex::basic))
	(L"EXTENDED"sv, static_cast<intptr_t>(std::wregex::extended))
	(L"AWK"sv, static_cast<intptr_t>(std::wregex::awk))
	(L"GREP"sv, static_cast<intptr_t>(std::wregex::grep))
	(L"EGREP"sv, static_cast<intptr_t>(std::wregex::egrep))
	(L"MULTILINE"sv, static_cast<intptr_t>(std::wregex::multiline))
	(L"search"sv, t_member<t_object*(*)(t_library*, const t_pattern&, const xemmaix::nata::t_text&, size_t, size_t), [](auto a_library, auto a_this, auto a_text, auto a_p, auto a_n)
	{
		size_t n = a_text.f_size();
		if ((a_p > n)) f_throw(L"out of range."sv);
		std::match_results<decltype(xemmaix::nata::t_text().f_begin())> m;
		std::regex_search(a_text.f_at(a_p), a_text.f_at(a_p + std::min(a_n, n - a_p)), m, a_this.v_pattern);
		return f_result(a_library, m);
	}>())
	.f_derive<t_pattern, xemmaix::nata::t_proxy>();
}

t_pvalue t_type_of<xemmaix::nata::t_pattern>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct<std::wstring_view, intptr_t>::t_bind<xemmaix::nata::t_pattern>::f_do(this, a_stack, a_n);
}

void t_type_of<xemmaix::nata::t_search>::f_define(t_library* a_library)
{
	using xemmaix::nata::t_search;
	t_define{a_library}
	(L"first"sv, t_member<t_object*(t_search::*)(t_library*, size_t, size_t), &t_search::f_first>())
	(L"next"sv, t_member<t_object*(t_search::*)(t_library*), &t_search::f_next>())
	.f_derive<t_search, xemmaix::nata::t_proxy>();
}

t_pvalue t_type_of<xemmaix::nata::t_search>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct<xemmaix::nata::t_text&, xemmaix::nata::t_pattern&>::t_bind<xemmaix::nata::t_search>::f_do(this, a_stack, a_n);
}

}
