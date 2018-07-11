#include "search.h"

namespace xemmaix::nata
{

void t_search::f_destroy()
{
	delete v_connection;
	v_text.f_release();
}

t_scoped t_search::f_next()
{
	t_scoped p = t_tuple::f_instantiate(v_i->size());
	auto& tuple = f_as<t_tuple&>(p);
	for (size_t i = 0; i < v_i->size(); ++i) {
		auto& m = (*v_i)[i];
		size_t j = m.first.f_index();
		tuple[i].f_construct(f_tuple(j, m.second.f_index() - j));
	}
	if (v_i != v_eos) ++v_i;
	return p;
}

}

namespace xemmai
{

void t_type_of<xemmaix::nata::t_search>::f_define(t_extension* a_extension)
{
	using xemmaix::nata::t_search;
	t_define<t_search, xemmaix::nata::t_proxy>(a_extension, L"Search")
		(L"ICASE", static_cast<intptr_t>(std::wregex::icase))
		(L"NOSUBS", static_cast<intptr_t>(std::wregex::nosubs))
		(L"OPTIMIZE", static_cast<intptr_t>(std::wregex::optimize))
		(L"COLLATE", static_cast<intptr_t>(std::wregex::collate))
		//(L"MULTILINE", static_cast<intptr_t>(std::wregex::multiline))
		(L"ECMASCRIPT", static_cast<intptr_t>(std::wregex::ECMAScript))
		(L"BASIC", static_cast<intptr_t>(std::wregex::basic))
		(L"EXTENDED", static_cast<intptr_t>(std::wregex::extended))
		(L"AWK", static_cast<intptr_t>(std::wregex::awk))
		(L"GREP", static_cast<intptr_t>(std::wregex::grep))
		(L"EGREP", static_cast<intptr_t>(std::wregex::egrep))
		(t_construct_with<t_scoped(*)(t_type*, xemmaix::nata::t_text&), t_search::f_construct>())
		(L"pattern", t_member<void(t_search::*)(const std::wstring&, intptr_t), &t_search::f_pattern>())
		(L"reset", t_member<void(t_search::*)(), &t_search::f_reset>())
		(L"next", t_member<t_scoped(t_search::*)(), &t_search::f_next>())
	;
}

t_scoped t_type_of<xemmaix::nata::t_search>::f_construct(t_stacked* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped(*)(t_type*, xemmaix::nata::t_text&), xemmaix::nata::t_search::f_construct>::t_bind<xemmaix::nata::t_search>::f_do(this, a_stack, a_n);
}

}
