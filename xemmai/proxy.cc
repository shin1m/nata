#include "proxy.h"

namespace xemmaix::nata
{

void t_proxy::f_dispose()
{
	v_object = nullptr;
	f_release();
	t_entry::f_dispose();
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

void t_type_of<xemmaix::nata::t_proxy>::f_do_finalize(t_object* a_this)
{
	auto p = static_cast<xemmaix::nata::t_proxy*>(a_this->f_pointer());
	assert(p->f_object() == nullptr);
	delete p;
}

t_scoped t_type_of<xemmaix::nata::t_proxy>::f_do_construct(t_stacked* a_stack, size_t a_n)
{
	f_throw(L"uninstantiatable.");
}

}
