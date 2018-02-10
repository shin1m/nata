#ifndef XEMMAIX__NATA__VIEW_H
#define XEMMAIX__NATA__VIEW_H

#include "../rows.h"
#include "../widget.h"
#include "text.h"

namespace xemmaix
{

namespace nata
{

template<typename T_target> struct t_overlay;

template<typename T_target>
struct t_view : t_proxy
{
	t_text& v_text;
	::nata::t_tokens<::nata::t_text<>, t_scoped>* v_tokens;
	T_target* v_target;
	::nata::t_rows<std::decay_t<decltype(*v_tokens)>, std::decay_t<decltype(*v_target)>, ::nata::t_foldable<>>* v_rows;
	::nata::t_widget<std::decay_t<decltype(*v_rows)>>* v_widget;
	std::vector<t_overlay<T_target>*> v_overlays;

	static t_scoped f_construct(t_object* a_class, t_text& a_text, size_t a_x, size_t a_y, size_t a_width, size_t a_height)
	{
		return (new t_view(a_class, a_text, a_x, a_y, a_width, a_height))->f_object();
	}
	static t_scoped f_tuple(const decltype(v_widget->v_line)& a_i)
	{
		t_scoped p = t_tuple::f_instantiate(5);
		auto& tuple = f_as<t_tuple&>(p);
		tuple[0].f_construct(t_value(a_i.v_i));
		tuple[1].f_construct(t_value(a_i.v_line));
		tuple[2].f_construct(t_value(a_i.v_text));
		tuple[3].f_construct(t_value(a_i.v_x));
		tuple[4].f_construct(t_value(a_i.v_y));
		return p;
	}

	t_view(t_object* a_class, t_text& a_text, size_t a_x, size_t a_y, size_t a_width, size_t a_height) : t_proxy(a_class), v_text(a_text), v_tokens(new std::decay_t<decltype(*v_tokens)>(*v_text.v_text)), v_target(new T_target(a_x, a_y, a_width, a_height)), v_rows(new std::decay_t<decltype(*v_rows)>(*v_tokens, *v_target)), v_widget(new std::decay_t<decltype(*v_widget)>(*v_rows))
	{
		v_text.f_acquire();
	}
	virtual void f_destroy()
	{
		delete v_widget;
		delete v_rows;
		delete v_target;
		delete v_tokens;
		v_text.f_release();
	}
	virtual void f_dispose()
	{
		while (!v_overlays.empty()) v_overlays.back()->f_dispose();
		t_proxy::f_dispose();
	}
	void f_move(size_t a_x, size_t a_y, size_t a_width, size_t a_height)
	{
		v_target->f_move(a_x, a_y, a_width, a_height);
	}
	void f_attributes(const typename T_target::t_attribute& a_control, const typename T_target::t_attribute& a_folded)
	{
		v_target->f_attributes(a_control, a_folded);
	}
	void f_paint(size_t a_p, size_t a_n, t_scoped&& a_token)
	{
		size_t n = v_text.f_size();
		if (a_p > n) t_throwable::f_throw(L"out of range.");
		v_tokens->f_paint(a_p, {{std::move(a_token), std::min(a_n, n - a_p)}});
	}
	void f_foldable(size_t a_p, size_t a_n, bool a_foldable)
	{
		size_t n = v_text.f_size();
		if (a_p > n) t_throwable::f_throw(L"out of range.");
		if (a_foldable)
			v_rows->f_foldable(a_p, {{{{std::min(a_n, n - a_p)}}}});
		else
			v_rows->f_foldable(a_p, {{std::min(a_n, n - a_p)}});
	}
	size_t f_folded(size_t a_p, bool a_folded)
	{
		if (a_p > v_text.f_size()) t_throwable::f_throw(L"out of range.");
		return v_rows->f_folded(a_p, a_folded);
	}
	void f_render()
	{
		{
			typename T_target::t_graphics g(*v_target);
			v_widget->f_render(g);
		}
		v_target->f_cursor(std::get<1>(v_widget->v_position) - v_widget->v_row.f_index().v_x, v_widget->v_row.f_index().v_y - v_widget->v_top);
	}
	t_scoped f_size() const
	{
		return f_tuple(v_rows->f_size());
	}
	size_t f_height() const
	{
		return v_widget->f_height();
	}
	size_t f_range() const
	{
		return v_widget->f_range();
	}
	size_t f_top() const
	{
		return v_widget->v_top;
	}
	t_scoped f_position() const
	{
		t_scoped p = t_tuple::f_instantiate(3);
		auto& tuple = f_as<t_tuple&>(p);
		tuple[0].f_construct(t_value(std::get<0>(v_widget->v_position)));
		tuple[1].f_construct(t_value(std::get<1>(v_widget->v_position)));
		tuple[2].f_construct(t_value(std::get<2>(v_widget->v_position)));
		return p;
	}
	void f_position__(size_t a_value, bool a_forward)
	{
		if (a_value > v_text.f_size()) t_throwable::f_throw(L"out of range.");
		v_widget->f_position__(a_value, a_forward);
	}
	t_scoped f_line() const
	{
		return f_tuple(v_widget->v_line);
	}
	void f_line__(size_t a_value)
	{
		if (a_value >= v_rows->f_size().v_line) t_throwable::f_throw(L"out of range.");
		v_widget->v_line.v_line = a_value;
		v_widget->f_from_line();
	}
	void f_into_view(size_t a_y, size_t a_height)
	{
		v_widget->f_into_view(a_y, a_height);
	}
	void f_into_view(size_t a_p)
	{
		v_widget->f_into_view(v_rows->f_at_in_text(a_p));
	}
	intptr_t f_get()
	{
		wint_t c;
		if (v_target->f_get(c) == ERR) t_throwable::f_throw(L"get_wch");
		return c;
	}
	void f_timeout(int a_delay)
	{
		v_target->f_timeout(a_delay);
	}
};

template<typename T_target>
struct t_overlay : t_proxy
{
	t_view<T_target>& v_view;
	std::decay_t<decltype(*v_view.v_widget->f_overlays()[0].second)>* v_overlay;

	static t_scoped f_construct(t_object* a_class, t_view<T_target>& a_view, const typename T_target::t_attribute& a_attribute)
	{
		return (new t_overlay(a_class, a_view, a_attribute))->f_object();
	}

	t_overlay(t_object* a_class, t_view<T_target>& a_view, const typename T_target::t_attribute& a_attribute) : t_proxy(a_class), v_view(a_view)
	{
		v_view.f_acquire();
		v_view.v_widget->f_add_overlay(a_attribute);
		v_overlay = v_view.v_widget->f_overlays().back().second.get();
		v_view.v_overlays.push_back(this);
	}
	virtual void f_destroy()
	{
		v_view.f_release();
	}
	virtual void f_dispose()
	{
		auto& overlays = v_view.v_overlays;
		auto i = std::find(overlays.begin(), overlays.end(), this);
		v_view.v_widget->f_remove_overlay(i - overlays.begin());
		overlays.erase(i);
		t_proxy::f_dispose();
	}
	void f_paint(size_t a_p, size_t a_n, bool a_on)
	{
		size_t n = v_view.v_text.f_size();
		if (a_p > n) t_throwable::f_throw(L"out of range.");
		v_overlay->f_paint(a_p, {{a_on, std::min(a_n, n - a_p)}});
	}
};

template<typename T_target>
struct t_overlay_iterator : t_proxy
{
	t_overlay<T_target>& v_overlay;
	typename std::decay_t<decltype(*v_overlay.v_overlay)>::t_iterator v_i;

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

	static t_scoped f_construct(t_object* a_class, t_overlay<T_target>& a_overlay)
	{
		return (new t_overlay_iterator(a_class, a_overlay))->f_object();
	}

	t_overlay_iterator(t_object* a_class, t_overlay<T_target>& a_overlay) : t_proxy(a_class), v_overlay(a_overlay), v_connection0(v_overlay.v_overlay->v_replaced >> v_replaced), v_connection1(v_overlay.v_overlay->v_painted >> v_painted)
	{
		v_overlay.f_acquire();
		v_i = v_overlay.v_overlay->f_begin();
	}
	virtual void f_destroy()
	{
		v_overlay.f_release();
	}
	virtual void f_dispose()
	{
		if (v_overlay.f_object()) {
			delete v_connection0;
			delete v_connection1;
		}
		t_proxy::f_dispose();
	}
	t_scoped f_next()
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
};

}

}

namespace xemmai
{

template<typename T_target>
struct t_type_of<xemmaix::nata::t_view<T_target>> : t_type_of<xemmaix::nata::t_proxy>
{
	typedef typename T_target::t_extension t_extension;
	using t_view = xemmaix::nata::t_view<T_target>;

	static void f_define(t_extension* a_extension)
	{
		t_define<t_view, xemmaix::nata::t_proxy>(a_extension, L"View")
			(t_construct_with<t_scoped(*)(t_object*, xemmaix::nata::t_text&, size_t, size_t, size_t, size_t), t_view::f_construct>())
			(L"move", t_member<void(t_view::*)(size_t, size_t, size_t, size_t), &t_view::f_move>())
			(L"attributes", t_member<void(t_view::*)(const typename T_target::t_attribute&, const typename T_target::t_attribute&), &t_view::f_attributes>())
			(L"paint", t_member<void(t_view::*)(size_t, size_t, t_scoped&&), &t_view::f_paint>())
			(L"foldable", t_member<void(t_view::*)(size_t, size_t, bool), &t_view::f_foldable>())
			(L"folded", t_member<size_t(t_view::*)(size_t, bool), &t_view::f_folded>())
			(L"render", t_member<void(t_view::*)(), &t_view::f_render>())
			(L"size", t_member<t_scoped(t_view::*)() const, &t_view::f_size>())
			(L"height", t_member<size_t(t_view::*)() const, &t_view::f_height>())
			(L"range", t_member<size_t(t_view::*)() const, &t_view::f_range>())
			(L"top", t_member<size_t(t_view::*)() const, &t_view::f_top>())
			(L"position", t_member<t_scoped(t_view::*)() const, &t_view::f_position>())
			(L"position__", t_member<void(t_view::*)(size_t, bool), &t_view::f_position__>())
			(L"line", t_member<t_scoped(t_view::*)() const, &t_view::f_line>())
			(L"line__", t_member<void(t_view::*)(size_t), &t_view::f_line__>())
			(L"into_view",
				t_member<void(t_view::*)(size_t, size_t), &t_view::f_into_view>(),
				t_member<void(t_view::*)(size_t), &t_view::f_into_view>()
			)
			(L"get", t_member<intptr_t(t_view::*)(), &t_view::f_get>())
			(L"timeout", t_member<void(t_view::*)(int), &t_view::f_timeout>())
		;
	}

	using t_type_of<xemmaix::nata::t_proxy>::t_type_of;
	virtual t_type* f_derive(t_object* a_this)
	{
		return new t_derived<t_type_of>(t_scoped(v_module), a_this);
	}
	virtual t_scoped f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n)
	{
		return t_construct_with<t_scoped(*)(t_object*, xemmaix::nata::t_text&, size_t, size_t, size_t, size_t), t_view::f_construct>::template t_bind<t_view>::f_do(a_class, a_stack, a_n);
	}
};

template<typename T_target>
struct t_type_of<xemmaix::nata::t_overlay<T_target>> : t_type_of<xemmaix::nata::t_proxy>
{
	typedef typename T_target::t_extension t_extension;
	using t_overlay = xemmaix::nata::t_overlay<T_target>;

	static void f_define(t_extension* a_extension)
	{
		t_define<t_overlay, xemmaix::nata::t_proxy>(a_extension, L"Overlay")
			(t_construct_with<t_scoped(*)(t_object*, xemmaix::nata::t_view<T_target>&, const typename T_target::t_attribute&), t_overlay::f_construct>())
			(L"paint", t_member<void(t_overlay::*)(size_t, size_t, bool), &t_overlay::f_paint>())
		;
	}

	using t_type_of<xemmaix::nata::t_proxy>::t_type_of;
	virtual t_type* f_derive(t_object* a_this)
	{
		return new t_derived<t_type_of>(t_scoped(v_module), a_this);
	}
	virtual t_scoped f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n)
	{
		return t_construct_with<t_scoped(*)(t_object*, xemmaix::nata::t_view<T_target>&, const typename T_target::t_attribute&), t_overlay::f_construct>::template t_bind<t_overlay>::f_do(a_class, a_stack, a_n);
	}
};

template<typename T_target>
struct t_type_of<xemmaix::nata::t_overlay_iterator<T_target>> : t_type_of<xemmaix::nata::t_proxy>
{
	typedef typename T_target::t_extension t_extension;
	using t_overlay_iterator = xemmaix::nata::t_overlay_iterator<T_target>;

	static void f_define(t_extension* a_extension)
	{
		t_define<t_overlay_iterator, xemmaix::nata::t_proxy>(a_extension, L"OverlayIterator")
			(t_construct_with<t_scoped(*)(t_object*, xemmaix::nata::t_overlay<T_target>&), t_overlay_iterator::f_construct>())
			(L"next", t_member<t_scoped(t_overlay_iterator::*)(), &t_overlay_iterator::f_next>())
		;
	}

	using t_type_of<xemmaix::nata::t_proxy>::t_type_of;
	virtual t_type* f_derive(t_object* a_this)
	{
		return new t_derived<t_type_of>(t_scoped(v_module), a_this);
	}
	virtual t_scoped f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n)
	{
		return t_construct_with<t_scoped(*)(t_object*, xemmaix::nata::t_overlay<T_target>&), t_overlay_iterator::f_construct>::template t_bind<t_overlay_iterator>::f_do(a_class, a_stack, a_n);
	}
};

}

#endif
