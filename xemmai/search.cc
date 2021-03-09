#include "search.h"

namespace xemmaix::nata
{

void t_search::f_destroy()
{
	delete v_connection;
	v_text.f_release();
}

t_pvalue t_search::f_next()
{
	return t_tuple::f_instantiate(v_i->size(), [&](auto& tuple)
	{
		for (size_t i = 0; i < v_i->size(); ++i) {
			auto& m = (*v_i)[i];
			size_t j = m.first.f_index();
			new(&tuple[i]) t_svalue(f_tuple(j, m.second.f_index() - j));
		}
		if (v_i != v_eos) ++v_i;
	});
}

}

namespace xemmai
{

void t_type_of<xemmaix::nata::t_search>::f_define(t_extension* a_extension)
{
	using xemmaix::nata::t_search;
	t_define<t_search, xemmaix::nata::t_proxy>(a_extension, L"Search"sv)
		(L"ICASE"sv, static_cast<intptr_t>(std::wregex::icase))
		(L"NOSUBS"sv, static_cast<intptr_t>(std::wregex::nosubs))
		(L"OPTIMIZE"sv, static_cast<intptr_t>(std::wregex::optimize))
		(L"COLLATE"sv, static_cast<intptr_t>(std::wregex::collate))
		//(L"MULTILINE"sv, static_cast<intptr_t>(std::wregex::multiline))
		(L"ECMASCRIPT"sv, static_cast<intptr_t>(std::wregex::ECMAScript))
		(L"BASIC"sv, static_cast<intptr_t>(std::wregex::basic))
		(L"EXTENDED"sv, static_cast<intptr_t>(std::wregex::extended))
		(L"AWK"sv, static_cast<intptr_t>(std::wregex::awk))
		(L"GREP"sv, static_cast<intptr_t>(std::wregex::grep))
		(L"EGREP"sv, static_cast<intptr_t>(std::wregex::egrep))
		(t_construct_with<t_pvalue(*)(t_type*, xemmaix::nata::t_text&), t_search::f_construct>())
		(L"pattern"sv, t_member<void(t_search::*)(std::wstring_view, intptr_t), &t_search::f_pattern>())
		(L"reset"sv, t_member<void(t_search::*)(), &t_search::f_reset>())
		(L"next"sv, t_member<t_pvalue(t_search::*)(), &t_search::f_next>())
	;
}

t_pvalue t_type_of<xemmaix::nata::t_search>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_pvalue(*)(t_type*, xemmaix::nata::t_text&), xemmaix::nata::t_search::f_construct>::t_bind<xemmaix::nata::t_search>::f_do(this, a_stack, a_n);
}

}
