#include "proxy.h"

namespace xemmaix::nata
{

void t_proxy::f_dispose()
{
	v_session = nullptr;
	f_release();
	this->~t_entry();
}

}

namespace xemmai
{

void t_type_of<xemmaix::nata::t_proxy>::f_define(t_library* a_library)
{
	using xemmaix::nata::t_proxy;
	t_define{a_library}
	(L"dispose"sv, t_member<void(t_proxy::*)(), &t_proxy::f_dispose>())
	.f_derive<t_proxy, t_object>();
}

t_pvalue t_type_of<xemmaix::nata::t_proxy>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	f_throw(L"uninstantiatable."sv);
}

}
