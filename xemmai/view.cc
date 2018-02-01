#include "view.h"
#include <xemmai/tuple.h>

namespace xemmaix
{

namespace nata
{

void t_view::f_destroy()
{
	delete v_widget;
	delete v_rows;
	delete v_tokens;
	v_text.f_release();
}

void t_view::f_dispose()
{
	while (!v_overlays.empty()) v_overlays.back()->f_dispose();
	t_proxy::f_dispose();
}

void t_overlay::f_destroy()
{
	v_view.f_release();
}

void t_overlay::f_dispose()
{
	auto& overlays = v_view.v_overlays;
	auto i = std::find(overlays.begin(), overlays.end(), this);
	v_view.v_widget->f_remove_overlay(i - overlays.begin());
	overlays.erase(i);
	t_proxy::f_dispose();
}

void t_overlay_iterator::f_destroy()
{
	v_overlay.f_release();
}

void t_overlay_iterator::f_dispose()
{
	if (v_overlay.f_object()) {
		delete v_connection0;
		delete v_connection1;
	}
	t_proxy::f_dispose();
}

t_scoped t_overlay_iterator::f_next()
{
	if (!v_overlay.f_object() || v_i == v_overlay.v_overlay->f_end()) return nullptr;
	t_scoped p = t_tuple::f_instantiate(3);
	auto& tuple = f_as<t_tuple&>(p);
	tuple[0].f_construct(t_value(v_i->v_x));
	tuple[1].f_construct(t_value(v_i.f_index().v_i1));
	tuple[2].f_construct(t_value(v_i.f_delta().v_i1));
	++v_i;
	return p;
}

}

}

namespace xemmai
{

void t_type_of<xemmaix::nata::t_view>::f_define(t_extension* a_extension)
{
	using xemmaix::nata::t_view;
	t_define<t_view, xemmaix::nata::t_proxy>(a_extension, L"View")
		(t_construct_with<t_scoped(*)(t_object*, xemmaix::nata::t_text&), xemmaix::nata::t_view::f_construct>())
		(L"resize", t_member<void(t_view::*)(), &t_view::f_resize>())
		(L"attributes", t_member<void(t_view::*)(attr_t, attr_t), &t_view::f_attributes>())
		(L"paint", t_member<void(t_view::*)(size_t, size_t, attr_t), &t_view::f_paint>())
		(L"foldable", t_member<void(t_view::*)(size_t, size_t, bool), &t_view::f_foldable>())
		(L"folded", t_member<size_t(t_view::*)(size_t, bool), &t_view::f_folded>())
		(L"render", t_member<void(t_view::*)(), &t_view::f_render>())
		(L"position", t_member<size_t(t_view::*)() const, &t_view::f_position>())
		(L"position__", t_member<void(t_view::*)(size_t, bool), &t_view::f_position__>())
		(L"line", t_member<size_t(t_view::*)() const, &t_view::f_line>())
		(L"line__", t_member<void(t_view::*)(size_t), &t_view::f_line__>())
		(L"rows", t_member<size_t(t_view::*)() const, &t_view::f_rows>())
		(L"row", t_member<size_t(t_view::*)() const, &t_view::f_row>())
		(L"into_view", t_member<void(t_view::*)(size_t), &t_view::f_into_view>())
		(L"message__", t_member<void(t_view::*)(const std::wstring&), &t_view::f_message__>())
	;
}

t_type* t_type_of<xemmaix::nata::t_view>::f_derive(t_object* a_this)
{
	return new t_derived<t_type_of>(t_scoped(v_module), a_this);
}

t_scoped t_type_of<xemmaix::nata::t_view>::f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped(*)(t_object*, xemmaix::nata::t_text&), xemmaix::nata::t_view::f_construct>::t_bind<xemmaix::nata::t_view>::f_do(a_class, a_stack, a_n);
}

void t_type_of<xemmaix::nata::t_overlay>::f_define(t_extension* a_extension)
{
	using xemmaix::nata::t_overlay;
	t_define<t_overlay, xemmaix::nata::t_proxy>(a_extension, L"Overlay")
		(t_construct_with<t_scoped(*)(t_object*, xemmaix::nata::t_view&, attr_t), xemmaix::nata::t_overlay::f_construct>())
		(L"paint", t_member<void(t_overlay::*)(size_t, size_t, bool), &t_overlay::f_paint>())
	;
}

t_type* t_type_of<xemmaix::nata::t_overlay>::f_derive(t_object* a_this)
{
	return new t_derived<t_type_of>(t_scoped(v_module), a_this);
}

t_scoped t_type_of<xemmaix::nata::t_overlay>::f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped(*)(t_object*, xemmaix::nata::t_view&, attr_t), xemmaix::nata::t_overlay::f_construct>::t_bind<xemmaix::nata::t_overlay>::f_do(a_class, a_stack, a_n);
}

void t_type_of<xemmaix::nata::t_overlay_iterator>::f_define(t_extension* a_extension)
{
	using xemmaix::nata::t_overlay_iterator;
	t_define<t_overlay_iterator, xemmaix::nata::t_proxy>(a_extension, L"OverlayIterator")
		(t_construct_with<t_scoped(*)(t_object*, xemmaix::nata::t_overlay&), xemmaix::nata::t_overlay_iterator::f_construct>())
		(L"next", t_member<t_scoped(t_overlay_iterator::*)(), &t_overlay_iterator::f_next>())
	;
}

t_type* t_type_of<xemmaix::nata::t_overlay_iterator>::f_derive(t_object* a_this)
{
	return new t_derived<t_type_of>(t_scoped(v_module), a_this);
}

t_scoped t_type_of<xemmaix::nata::t_overlay_iterator>::f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped(*)(t_object*, xemmaix::nata::t_overlay&), xemmaix::nata::t_overlay_iterator::f_construct>::t_bind<xemmaix::nata::t_overlay_iterator>::f_do(a_class, a_stack, a_n);
}

}
