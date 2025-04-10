#ifndef XEMMAIX__NATA__TEXT_H
#define XEMMAIX__NATA__TEXT_H

#include <nata/model.h>
#include "proxy.h"

namespace xemmaix::nata
{

struct t_text : t_proxy, ::nata::t_text<>
{
	static t_object* f_line(t_library* a_library, const auto& a_line)
	{
		auto i = a_line.f_index();
		return f_new_value(a_library->f_type_line(), i.v_i0, i.v_i1, a_line.f_delta().v_i1);
	}

	virtual void f_destroy();
	size_t f_lines() const
	{
		return ::nata::t_text<>::f_lines().f_size().v_i0;
	}
};

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::nata::t_text> : t_derivable<t_bears<xemmaix::nata::t_text, t_type_of<xemmaix::nata::t_proxy>>>
{
	static void f_define(t_library* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif
