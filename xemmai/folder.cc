#include "folder.h"
#include <xemmai/tuple.h>

namespace xemmaix
{

namespace nata
{

void t_folder::f_destroy()
{
	delete v_connection;
	delete v_folder;
	v_view.f_release();
}

}

}

namespace xemmai
{

void t_type_of<xemmaix::nata::t_folder>::f_define(t_extension* a_extension)
{
	using xemmaix::nata::t_folder;
	t_define<t_folder, xemmaix::nata::t_proxy>(a_extension, L"Folder")
		(t_construct_with<t_scoped(*)(t_object*, xemmaix::nata::t_view&), xemmaix::nata::t_folder::f_construct>())
		(L"reset", t_member<void(t_folder::*)(), &t_folder::f_reset>())
		(L"current", t_member<size_t(t_folder::*)() const, &t_folder::f_current>())
		(L"push", t_member<void(t_folder::*)(size_t), &t_folder::f_push>())
		(L"tag", t_member<t_scoped(t_folder::*)() const, &t_folder::f_tag>())
		(L"open", t_member<void(t_folder::*)(t_scoped&&), &t_folder::f_open>())
		(L"close", t_member<void(t_folder::*)(), &t_folder::f_close>())
		(L"flush", t_member<void(t_folder::*)(), &t_folder::f_flush>())
	;
}

t_type* t_type_of<xemmaix::nata::t_folder>::f_derive(t_object* a_this)
{
	return new t_derived<t_type_of>(t_scoped(v_module), a_this);
}

t_scoped t_type_of<xemmaix::nata::t_folder>::f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped(*)(t_object*, xemmaix::nata::t_view&), xemmaix::nata::t_folder::f_construct>::t_bind<xemmaix::nata::t_folder>::f_do(a_class, a_stack, a_n);
}

}
