#include "painter.h"
#include <xemmai/tuple.h>

namespace xemmaix
{

namespace nata
{

void t_painter::f_destroy()
{
	delete v_connection;
	delete v_painter;
	v_view.f_release();
}

}

}

namespace xemmai
{

void t_type_of<xemmaix::nata::t_painter>::f_define(t_extension* a_extension)
{
	using xemmaix::nata::t_painter;
	t_define<t_painter, xemmaix::nata::t_proxy>(a_extension, L"Painter")
		(t_construct_with<t_scoped(*)(t_object*, xemmaix::nata::t_view&), xemmaix::nata::t_painter::f_construct>())
		(L"reset", t_member<void(t_painter::*)(), &t_painter::f_reset>())
		(L"current", t_member<size_t(t_painter::*)() const, &t_painter::f_current>())
		(L"push",
			t_member<void(t_painter::*)(const t_painter::t_value&, size_t), &t_painter::f_push>(),
			t_member<void(t_painter::*)(const t_painter::t_value&, size_t, size_t), &t_painter::f_push>()
		)
		(L"flush", t_member<void(t_painter::*)(), &t_painter::f_flush>())
	;
}

t_type* t_type_of<xemmaix::nata::t_painter>::f_derive(t_object* a_this)
{
	return new t_derived<t_type_of>(t_scoped(v_module), a_this);
}

t_scoped t_type_of<xemmaix::nata::t_painter>::f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped(*)(t_object*, xemmaix::nata::t_view&), xemmaix::nata::t_painter::f_construct>::t_bind<xemmaix::nata::t_painter>::f_do(a_class, a_stack, a_n);
}

}
