#include "search.h"

namespace xemmaix::nata
{

void t_search::f_destroy()
{
	delete v_connection;
	v_pattern.~basic_regex();
	v_eos.~regex_iterator();
	v_i.~regex_iterator();
	v_text.f_release();
}

t_object* t_search::f_next(t_library* a_library)
{
	return t_tuple::f_instantiate(v_i->size(), [&](auto& tuple)
	{
		for (size_t i = 0; i < v_i->size(); ++i) {
			auto& m = (*v_i)[i];
			size_t j = m.first.f_index();
			new(&tuple[i]) t_svalue(f_new_value(a_library->f_type_span(), j, m.second.f_index() - j));
		}
		if (v_i != v_eos) ++v_i;
	});
}

}

namespace xemmai
{

void t_type_of<xemmaix::nata::t_search>::f_define(t_library* a_library)
{
	using xemmaix::nata::t_search;
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
	(L"pattern"sv, t_member<void(*)(t_search&, std::wstring_view, intptr_t), [](auto a_this, auto a_pattern, auto a_flags)
	{
		a_this.v_pattern.assign(a_pattern.begin(), a_pattern.end(), static_cast<std::wregex::flag_type>(a_flags));
	}>())
	(L"reset"sv, t_member<void(t_search::*)(size_t, size_t), &t_search::f_reset>())
	(L"next"sv, t_member<t_object*(t_search::*)(t_library*), &t_search::f_next>())
	.f_derive<t_search, xemmaix::nata::t_proxy>();
}

t_pvalue t_type_of<xemmaix::nata::t_search>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_object*(*)(t_type*, xemmaix::nata::t_text&), [](auto a_class, auto a_text)
	{
		return a_class->template f_new<xemmaix::nata::t_search>(a_text);
	}>::t_bind<xemmaix::nata::t_search>::f_do(this, a_stack, a_n);
}

}
