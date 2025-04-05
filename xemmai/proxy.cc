#include "proxy.h"

namespace xemmaix::nata
{

void t_proxy::f_dispose()
{
	t_entry::f_dispose();
	v_session = nullptr;
	f_release();
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

void t_type_of<xemmaix::nata::t_proxy>::f_do_finalize(t_object* a_this)
{
	auto& p = a_this->f_as<xemmaix::nata::t_proxy>();
	assert(!p.v_session);
	p.~t_proxy();
}

t_pvalue t_type_of<xemmaix::nata::t_proxy>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	f_throw(L"uninstantiatable."sv);
}

}
