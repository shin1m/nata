#ifndef XEMMAIX__NATA__FOLDER_H
#define XEMMAIX__NATA__FOLDER_H

#include "../folder.h"
#include "view.h"

namespace xemmaix
{

namespace nata
{

struct t_folder : t_proxy
{
	t_view& v_view;
	::nata::t_folder<std::decay_t<decltype(*t_view::v_rows)>, t_scoped>* v_folder;

	::nata::t_slot<size_t, size_t, size_t> v_replaced = [this](auto, auto, auto)
	{
		f_reset();
	};
	::nata::t_connection<decltype(v_replaced)>* v_connection;

	static t_scoped f_construct(t_object* a_class, t_view& a_view)
	{
		return (new t_folder(a_class, a_view))->f_object();
	}

	t_folder(t_object* a_class, t_view& a_view) : t_proxy(a_class), v_view(a_view), v_folder(new std::decay_t<decltype(*v_folder)>(*v_view.v_rows)), v_connection(v_view.v_text.v_text->v_replaced >> v_replaced)
	{
		v_view.f_acquire();
	}
	virtual void f_destroy();
	void f_reset()
	{
		v_folder->f_reset();
	}
	size_t f_current() const
	{
		return v_folder->f_current();
	}
	void f_push(size_t a_n)
	{
		v_folder->f_push(std::min(a_n, v_view.v_text.v_text->f_size() - f_current()));
	}
	t_scoped f_tag() const
	{
		return v_folder->f_tag();
	}
	void f_open(t_scoped&& a_tag)
	{
		v_folder->f_open(std::move(a_tag));
	}
	void f_close()
	{
		v_folder->f_close();
	}
	void f_flush()
	{
		v_folder->f_flush();
	}
};

}

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::nata::t_folder> : t_type_of<xemmaix::nata::t_proxy>
{
	static void f_define(t_extension* a_extension);

	using t_type_of<xemmaix::nata::t_proxy>::t_type_of;
	virtual t_type* f_derive(t_object* a_this);
	virtual t_scoped f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n);
};

}

#endif
