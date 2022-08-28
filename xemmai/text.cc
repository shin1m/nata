#include "text.h"

namespace xemmaix::nata
{

void t_text::f_destroy()
{
	delete v_text;
}

}

namespace xemmai
{

void t_type_of<xemmaix::nata::t_text>::f_define(t_library* a_library)
{
	using xemmaix::nata::t_text;
	t_define{a_library}
		(L"size"sv, t_member<size_t(t_text::*)() const, &t_text::f_size>())
		(L"slice"sv, t_member<std::wstring(t_text::*)(size_t, size_t) const, &t_text::f_slice>())
		(L"replace"sv, t_member<void(t_text::*)(size_t, size_t, std::wstring_view), &t_text::f_replace>())
		(L"lines"sv, t_member<size_t(t_text::*)() const, &t_text::f_lines>())
		(L"line_at"sv, t_member<t_pvalue(t_text::*)(t_library*, size_t) const, &t_text::f_line_at>())
		(L"line_at_in_text"sv, t_member<t_pvalue(t_text::*)(t_library*, size_t) const, &t_text::f_line_at_in_text>())
	.f_derive<t_text, xemmaix::nata::t_proxy>();
}

t_pvalue t_type_of<xemmaix::nata::t_text>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_pvalue(*)(t_type*), xemmaix::nata::t_text::f_construct>::t_bind<xemmaix::nata::t_text>::f_do(this, a_stack, a_n);
}

}
