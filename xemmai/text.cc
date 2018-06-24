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

void t_type_of<xemmaix::nata::t_text>::f_define(t_extension* a_extension)
{
	using xemmaix::nata::t_text;
	t_define<t_text, xemmaix::nata::t_proxy>(a_extension, L"Text")
		(t_construct_with<t_scoped(*)(t_type*), t_text::f_construct>())
		(L"size", t_member<size_t(t_text::*)() const, &t_text::f_size>())
		(L"slice", t_member<std::wstring(t_text::*)(size_t, size_t) const, &t_text::f_slice>())
		(L"replace", t_member<void(t_text::*)(size_t, size_t, const std::wstring&), &t_text::f_replace>())
		(L"lines", t_member<size_t(t_text::*)() const, &t_text::f_lines>())
		(L"line_at_in_text", t_member<t_scoped(t_text::*)(size_t) const, &t_text::f_line_at_in_text>())
	;
}

t_type* t_type_of<xemmaix::nata::t_text>::f_derive()
{
	return new t_derived<t_type_of>(t_scoped(v_module), this);
}

t_scoped t_type_of<xemmaix::nata::t_text>::f_construct(t_stacked* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped(*)(t_type*), xemmaix::nata::t_text::f_construct>::t_bind<xemmaix::nata::t_text>::f_do(this, a_stack, a_n);
}

}
