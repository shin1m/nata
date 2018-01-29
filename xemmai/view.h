#ifndef XEMMAIX__NATA__VIEW_H
#define XEMMAIX__NATA__VIEW_H

#include "../rows.h"
#include "../widget.h"
#include "../curses.h"
#include "text.h"

namespace xemmaix
{

namespace nata
{

struct t_overlay;

struct t_view : t_proxy
{
	t_text& v_text;
	::nata::t_tokens<::nata::t_text<>, attr_t>* v_tokens;
	::nata::curses::t_target v_target;
	::nata::t_rows<std::decay_t<decltype(*v_tokens)>, decltype(v_target), ::nata::t_foldable<>>* v_rows;
	::nata::t_widget<std::decay_t<decltype(*v_rows)>>* v_widget;
	std::vector<t_overlay*> v_overlays;
	attr_t v_attribute_control = A_NORMAL;
	attr_t v_attribute_folded = A_NORMAL;
	wchar_t v_prefix = L'\0';

	static t_scoped f_construct(t_object* a_class, t_text& a_text)
	{
		return (new t_view(a_class, a_text))->f_object();
	}

	t_view(t_object* a_class, t_text& a_text) : t_proxy(a_class), v_text(a_text), v_tokens(new std::decay_t<decltype(*v_tokens)>(*v_text.v_text)), v_rows(new std::decay_t<decltype(*v_rows)>(*v_tokens, v_target)), v_widget(new std::decay_t<decltype(*v_widget)>(*v_rows, LINES - 1))
	{
		v_text.f_acquire();
	}
	virtual void f_destroy();
	virtual void f_dispose();
	void f_resize()
	{
		v_widget->f_height__(LINES - 1);
		v_target.v_resized();
	}
	void f_attributes(attr_t a_control, attr_t a_folded)
	{
		v_attribute_control = a_control;
		v_attribute_folded = a_folded;
	}
	void f_paint(size_t a_p, size_t a_n, attr_t a_attribute)
	{
		v_tokens->f_paint(a_p, {{a_attribute, a_n}});
	}
	void f_foldable(size_t a_p, size_t a_n, bool a_foldable)
	{
		if (a_foldable)
			v_rows->f_foldable(a_p, {{{{a_n}}}});
		else
			v_rows->f_foldable(a_p, {{a_n}});
	}
	size_t f_folded(size_t a_p, bool a_folded)
	{
		if (a_p > v_text.f_size()) t_throwable::f_throw(L"out of range.");
		return v_rows->f_folded(a_p, a_folded);
	}
	void f_render()
	{
		{
			::nata::curses::t_graphics g{v_target, v_attribute_control, v_attribute_folded, 0, L'0' + v_prefix};
			v_widget->f_render(g);
			v_prefix = (v_prefix + 1) % 10;
		}
		size_t position = std::get<0>(v_widget->v_position);
		{
			auto line = v_text.v_text->f_lines().f_at_in_text(position).f_index();
			size_t column = position - line.v_i1;
			size_t x = std::get<1>(v_widget->v_position) - v_widget->v_line.v_x;
			size_t n = v_widget->f_range();
			mvprintw(v_widget->f_height(), 0, "%d,%d-%d %d%%", line.v_i0, column, x, n > 0 ? v_widget->v_top * 100 / n : 100);
		}
		clrtobot();
		v_target.f_move(std::get<1>(v_widget->v_position) - v_widget->v_row.f_index().v_x, v_widget->v_row.f_index().v_y - v_widget->v_top);
		refresh();
	}
	size_t f_position() const
	{
		return std::get<0>(v_widget->v_position);
	}
	void f_position__(size_t a_value, bool a_forward)
	{
		if (a_value > v_text.f_size()) t_throwable::f_throw(L"out of range.");
		v_widget->f_position__(a_value, a_forward);
	}
	size_t f_line() const
	{
		return v_widget->v_line.v_line;
	}
	void f_line__(size_t a_value)
	{
		if (a_value >= v_rows->f_size().v_line) t_throwable::f_throw(L"out of range.");
		v_widget->v_line.v_line = a_value;
		v_widget->f_from_line();
	}
	size_t f_rows() const
	{
		return v_rows->f_size().v_i;
	}
	size_t f_row() const
	{
		return v_widget->v_row.f_index().v_i;
	}
	void f_into_view(size_t a_row)
	{
		if (a_row >= f_rows()) t_throwable::f_throw(L"out of range.");
		v_widget->f_into_view(v_rows->f_at(a_row));
	}
};

struct t_overlay : t_proxy
{
	t_view& v_view;
	std::decay_t<decltype(*v_view.v_widget->f_overlays()[0].second)>* v_overlay;

	static t_scoped f_construct(t_object* a_class, t_view& a_view, attr_t a_attribute)
	{
		return (new t_overlay(a_class, a_view, a_attribute))->f_object();
	}

	t_overlay(t_object* a_class, t_view& a_view, attr_t a_attribute) : t_proxy(a_class), v_view(a_view)
	{
		v_view.f_acquire();
		v_view.v_widget->f_add_overlay(a_attribute);
		v_overlay = v_view.v_widget->f_overlays().back().second.get();
		v_view.v_overlays.push_back(this);
	}
	virtual void f_destroy();
	virtual void f_dispose();
	void f_paint(size_t a_p, size_t a_n, bool a_on)
	{
		size_t n = v_view.v_text.f_size();
		if (a_p > n) t_throwable::f_throw(L"out of range.");
		v_overlay->f_paint(a_p, {{a_on, std::min(a_n, n - a_p)}});
	}
};

struct t_overlay_iterator : t_proxy
{
	t_overlay& v_overlay;
	std::decay_t<decltype(*v_overlay.v_overlay)>::t_iterator v_i;

	::nata::t_slot<size_t, size_t, size_t> v_replaced = [this](auto, auto, auto)
	{
		v_i = v_overlay.v_overlay->f_end();
	};
	::nata::t_connection<decltype(v_replaced)>* v_connection0;
	::nata::t_slot<size_t, size_t> v_painted = [this](auto, auto)
	{
		v_i = v_overlay.v_overlay->f_end();
	};
	::nata::t_connection<decltype(v_painted)>* v_connection1;

	static t_scoped f_construct(t_object* a_class, t_overlay& a_overlay)
	{
		return (new t_overlay_iterator(a_class, a_overlay))->f_object();
	}

	t_overlay_iterator(t_object* a_class, t_overlay& a_overlay) : t_proxy(a_class), v_overlay(a_overlay), v_connection0(v_overlay.v_overlay->v_replaced >> v_replaced), v_connection1(v_overlay.v_overlay->v_painted >> v_painted)
	{
		v_overlay.f_acquire();
		v_i = v_overlay.v_overlay->f_begin();
	}
	virtual void f_destroy();
	virtual void f_dispose();
	t_scoped f_next();
};

}

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::nata::t_view> : t_type_of<xemmaix::nata::t_proxy>
{
	static void f_define(t_extension* a_extension);

	using t_type_of<xemmaix::nata::t_proxy>::t_type_of;
	virtual t_type* f_derive(t_object* a_this);
	virtual t_scoped f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n);
};

template<>
struct t_type_of<xemmaix::nata::t_overlay> : t_type_of<xemmaix::nata::t_proxy>
{
	static void f_define(t_extension* a_extension);

	using t_type_of<xemmaix::nata::t_proxy>::t_type_of;
	virtual t_type* f_derive(t_object* a_this);
	virtual t_scoped f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n);
};

template<>
struct t_type_of<xemmaix::nata::t_overlay_iterator> : t_type_of<xemmaix::nata::t_proxy>
{
	static void f_define(t_extension* a_extension);

	using t_type_of<xemmaix::nata::t_proxy>::t_type_of;
	virtual t_type* f_derive(t_object* a_this);
	virtual t_scoped f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n);
};

}

#endif
