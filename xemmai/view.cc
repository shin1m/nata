#include "view.h"

namespace xemmaix
{

namespace nata
{

void t_view::f_destroy()
{
	delete v_widget;
	delete v_rows;
	delete v_tokens;
	--v_text.v_n;
	t_proxy::f_destroy();
}

}

}

namespace xemmai
{

void t_type_of<xemmaix::nata::t_view>::f_define(t_extension* a_extension)
{
	using xemmaix::nata::t_view;
	t_define<t_view, t_object>(a_extension, L"View")
		(t_construct_with<t_scoped(*)(t_object*, xemmaix::nata::t_text&), xemmaix::nata::t_view::f_construct>())
		(L"destroy", t_member<void(t_view::*)(), &t_view::f_destroy>())
		(L"resize", t_member<void(t_view::*)(), &t_view::f_resize>())
		(L"attributes", t_member<void(t_view::*)(attr_t, attr_t), &t_view::f_attributes>())
		(L"paint", t_member<void(t_view::*)(size_t, attr_t), &t_view::f_paint>())
		(L"foldable", t_member<void(t_view::*)(size_t, bool), &t_view::f_foldable>())
		(L"folded", t_member<size_t(t_view::*)(size_t, bool), &t_view::f_folded>())
		(L"overlays", t_member<size_t(t_view::*)() const, &t_view::f_overlays>())
		(L"add_overlay", t_member<void(t_view::*)(attr_t), &t_view::f_add_overlay>())
		(L"remove_overlay", t_member<void(t_view::*)(size_t), &t_view::f_remove_overlay>())
		(L"overlay", t_member<void(t_view::*)(size_t, size_t, size_t, bool), &t_view::f_overlay>())
		(L"render", t_member<void(t_view::*)(), &t_view::f_render>())
		(L"position", t_member<size_t(t_view::*)() const, &t_view::f_position>())
		(L"position__", t_member<void(t_view::*)(size_t, bool), &t_view::f_position__>())
		(L"line", t_member<size_t(t_view::*)() const, &t_view::f_line>())
		(L"line__", t_member<void(t_view::*)(size_t), &t_view::f_line__>())
		(L"rows", t_member<size_t(t_view::*)() const, &t_view::f_rows>())
		(L"row", t_member<size_t(t_view::*)() const, &t_view::f_row>())
		(L"into_view", t_member<void(t_view::*)(size_t), &t_view::f_into_view>())
	;
}

t_type* t_type_of<xemmaix::nata::t_view>::f_derive(t_object* a_this)
{
	return new t_derived<t_type_of>(t_scoped(v_module), a_this);
}

void t_type_of<xemmaix::nata::t_view>::f_finalize(t_object* a_this)
{
	auto p = static_cast<xemmaix::nata::t_view*>(a_this->f_pointer());
	assert(!p->f_object());
	delete p;
}

t_scoped t_type_of<xemmaix::nata::t_view>::f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped(*)(t_object*, xemmaix::nata::t_text&), xemmaix::nata::t_view::f_construct>::t_bind<xemmaix::nata::t_view>::f_do(a_class, a_stack, a_n);
}

}
