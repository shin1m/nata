#include "proxy.h"

namespace xemmaix
{

namespace nata
{

void t_proxy::f_dispose()
{
	v_object = nullptr;
	f_release();
	t_entry::f_dispose();
}

}

}

namespace xemmai
{

void t_type_of<xemmaix::nata::t_proxy>::f_define(t_extension* a_extension)
{
	using xemmaix::nata::t_proxy;
	t_define<t_proxy, t_object>(a_extension, L"Proxy")
		(L"dispose", t_member<void(t_proxy::*)(), &t_proxy::f_dispose>())
	;
}

t_type* t_type_of<xemmaix::nata::t_proxy>::f_derive(t_object* a_this)
{
	return nullptr;
}

void t_type_of<xemmaix::nata::t_proxy>::f_finalize(t_object* a_this)
{
	auto p = static_cast<xemmaix::nata::t_proxy*>(a_this->f_pointer());
	assert(!p->f_object());
	delete p;
}

t_scoped t_type_of<xemmaix::nata::t_proxy>::f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

}
