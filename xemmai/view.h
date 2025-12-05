#ifndef XEMMAIX__NATA__VIEW_H
#define XEMMAIX__NATA__VIEW_H

#include <nata/rows.h>
#include <nata/widget.h>
#include "text.h"

namespace xemmaix::nata
{

template<typename> struct t_overlay;

template<typename T_target>
struct t_view : t_proxy, T_target
{
	t_text& v_text;
	::nata::t_tokens<::nata::t_text<>, t_rvalue> v_tokens;
	::nata::t_rows<decltype(v_tokens), T_target> v_rows;
	::nata::t_widget<decltype(v_rows)> v_widget;
	std::vector<t_overlay<T_target>*> v_overlays;

	static t_object* f_row(typename T_target::t_library* a_library, const auto& a_row)
	{
		return f_new_value(a_library->f_type_row(), a_row.v_i, a_row.v_line, a_row.v_text, a_row.v_x, a_row.v_y);
	}
	static t_object* f_row(typename T_target::t_library* a_library, const decltype(v_rows.f_begin())& a_i)
	{
		auto index = a_i.f_index();
		index.v_line += a_i.f_delta().v_line;
		--index.v_line;
		return f_row(a_library, index);
	}

	t_view(t_text& a_text, size_t a_x, size_t a_y, size_t a_width, size_t a_height) : T_target(a_x, a_y, a_width, a_height), v_text(a_text), v_tokens(v_text), v_rows(v_tokens, *this), v_widget(v_rows)
	{
		v_text.f_acquire();
	}
	virtual void f_destroy()
	{
		v_overlays.~vector();
		v_widget.~t_widget();
		v_rows.~t_rows();
		v_tokens.~t_tokens();
		v_text.f_release();
		this->~T_target();
	}
	virtual void f_dispose()
	{
		while (!v_overlays.empty()) v_overlays.back()->f_dispose();
		t_proxy::f_dispose();
	}
};

template<typename T_target>
struct t_overlay : t_proxy
{
	t_view<T_target>& v_view;
	std::decay_t<decltype(*v_view.v_widget.f_overlays()[0].second)>* v_overlay;

	t_overlay(t_view<T_target>& a_view, const typename T_target::t_attribute& a_attribute) : v_view(a_view)
	{
		v_view.f_acquire();
		v_view.v_widget.f_add_overlay(a_attribute);
		v_overlay = v_view.v_widget.f_overlays().back().second.get();
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
		v_view.v_widget.f_remove_overlay(i - overlays.begin());
		overlays.erase(i);
		t_proxy::f_dispose();
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

	t_overlay_iterator(t_overlay<T_target>& a_overlay) : v_overlay(a_overlay), v_connection0(v_overlay.v_overlay->v_replaced >> v_replaced), v_connection1(v_overlay.v_overlay->v_painted >> v_painted)
	{
		v_overlay.f_acquire();
		v_i = v_overlay.v_overlay->f_begin();
	}
	virtual void f_destroy()
	{
		v_i.~t_iterator();
		v_overlay.f_release();
	}
	virtual void f_dispose()
	{
		if (v_overlay.f_valid()) {
			delete v_connection0;
			delete v_connection1;
		}
		t_proxy::f_dispose();
	}
};

}

namespace xemmai
{

template<typename T_target>
struct t_type_of<xemmaix::nata::t_view<T_target>> : t_derivable<t_bears<xemmaix::nata::t_view<T_target>, t_type_of<xemmaix::nata::t_proxy>>>
{
	using t_library = typename T_target::t_library;
	using t_view = xemmaix::nata::t_view<T_target>;

	static void f_define(t_library* a_library)
	{
		t_define{a_library}
		(L"move"sv, t_member<void(*)(t_view&, size_t, size_t, size_t, size_t), [](auto a_this, auto a_x, auto a_y, auto a_width, auto a_height)
		{
			a_this.f_move(a_x, a_y, a_width, a_height);
		}>())
		(L"attributes"sv, t_member<void(*)(t_view&, const typename T_target::t_attribute&, const typename T_target::t_attribute&), [](auto a_this, auto a_control, auto a_folded)
		{
			a_this.f_attributes(a_control, a_folded);
		}>())
		(L"paint"sv, t_member<void(*)(t_view&, size_t, size_t, const t_pvalue&), [](auto a_this, auto a_p, auto a_n, auto a_token)
		{
			size_t n = a_this.v_text.f_size();
			if ((a_p > n)) f_throw(L"out of range."sv);
			a_this.v_tokens.f_paint(a_p, {{std::move(a_token), std::min(a_n, n - a_p)}});
		}>())
		(L"crease"sv, t_member<void(*)(t_view&, size_t, size_t, bool), [](auto a_this, auto a_p, auto a_n, auto a_on)
		{
			size_t n = a_this.v_text.f_size();
			if ((a_p > n)) f_throw(L"out of range."sv);
			if (a_on)
				a_this.v_rows.f_crease(a_p, {{{{std::min(a_n, n - a_p)}}}});
			else
				a_this.v_rows.f_crease(a_p, {{std::min(a_n, n - a_p)}});
		}>())
		(L"folded"sv, t_member<size_t(*)(t_view&, size_t, bool), [](auto a_this, auto a_p, auto a_on)
		{
			if ((a_p > a_this.v_text.f_size())) f_throw(L"out of range."sv);
			return a_this.v_rows.f_folded(a_p, a_on);
		}>())
		(L"render"sv, t_member<void(*)(t_view&), [](auto a_this)
		{
			typename T_target::t_graphics g(a_this);
			a_this.v_widget.f_render(g);
		}>())
		(L"focus"sv, t_member<void(*)(t_view&), [](auto a_this)
		{
			auto& widget = a_this.v_widget;
			auto& row = widget.v_row.f_index();
			a_this.f_cursor(std::get<1>(widget.v_position) - row.v_x, row.v_y - widget.v_top);
		}>())
		(L"size"sv, t_member<t_object*(*)(t_library*, const t_view&), [](auto a_library, auto a_this)
		{
			return t_view::f_row(a_library, a_this.v_rows.f_size());
		}>())
		(L"height"sv, t_member<size_t(*)(const t_view&), [](auto a_this)
		{
			return a_this.v_widget.f_height();
		}>())
		(L"range"sv, t_member<size_t(*)(const t_view&), [](auto a_this)
		{
			return a_this.v_widget.f_range();
		}>())
		(L"top"sv, t_member<size_t(*)(const t_view&), [](auto a_this)
		{
			return a_this.v_widget.v_top;
		}>())
		(L"position"sv, t_member<t_object*(*)(t_library*, const t_view&), [](auto a_library, auto a_this)
		{
			auto [text, x, width] = a_this.v_widget.v_position;
			return f_new_value(a_library->f_type_position(), text, x, width);
		}>())
		(L"position__"sv, t_member<void(*)(t_view&, size_t, bool), [](auto a_this, auto a_value, auto a_forward)
		{
			if ((a_value > a_this.v_text.f_size())) f_throw(L"out of range."sv);
			a_this.v_widget.f_position__(a_value, a_forward);
		}>())
		(L"row"sv, t_member<t_object*(*)(t_library*, const t_view&), [](auto a_library, auto a_this)
		{
			return t_view::f_row(a_library, a_this.v_widget.v_line);
		}>())
		(L"row_at"sv, t_member<t_object*(*)(t_library*, const t_view&, size_t), [](auto a_library, auto a_this, auto a_p)
		{
			return t_view::f_row(a_library, a_this.v_rows.f_at(a_p));
		}>())
		(L"row_at_line"sv, t_member<t_object*(*)(t_library*, const t_view&, size_t), [](auto a_library, auto a_this, auto a_p)
		{
			return t_view::f_row(a_library, a_this.v_rows.f_at_line(a_p).f_index());
		}>())
		(L"row_at_y"sv, t_member<t_object*(*)(t_library*, const t_view&, size_t), [](auto a_library, auto a_this, auto a_p)
		{
			return t_view::f_row(a_library, a_this.v_rows.f_at_y(a_p));
		}>())
		(L"line__"sv, t_member<void(*)(t_view&, size_t), [](auto a_this, auto a_value)
		{
			if (a_value >= a_this.v_rows.f_size().v_line) f_throw(L"out of range."sv);
			a_this.v_widget.v_line.v_line = a_value;
			a_this.v_widget.f_from_line();
		}>())
		(L"target"sv, t_member<size_t(*)(const t_view&), [](auto a_this)
		{
			return a_this.v_widget.v_target;
		}>())
		(L"target__"sv, t_member<void(*)(t_view&, size_t), [](auto a_this, auto a_value)
		{
			a_this.v_widget.v_target = a_value;
			a_this.v_widget.f_from_line();
		}>())
		(L"into_view"sv,
			t_member<void(*)(t_view&, size_t, size_t), [](auto a_this, auto a_y, auto a_height)
			{
				a_this.v_widget.f_into_view(a_y, a_height);
			}>(),
			t_member<void(*)(t_view&, size_t), [](auto a_this, auto a_p)
			{
				a_this.v_widget.f_into_view(a_this.v_rows.f_at_text(a_p));
			}>()
		)
		(L"timeout"sv, t_member<void(*)(t_view&, int), [](auto a_this, auto a_delay)
		{
			a_this.f_timeout(a_delay);
		}>())
		(L"get"sv, t_member<intptr_t(*)(t_view&), [](auto a_this)
		{
			wint_t c;
			if (a_this.f_get(c) == ERR) f_throw(L"get_wch"sv);
			return static_cast<intptr_t>(c);
		}>())
		.template f_derive<t_view, xemmaix::nata::t_proxy>();
	}

	using t_type_of::t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n)
	{
		return t_construct<xemmaix::nata::t_text&, size_t, size_t, size_t, size_t>::template t_bind<t_view>::f_do(this, a_stack, a_n);
	}
};

template<typename T_target>
struct t_type_of<xemmaix::nata::t_overlay<T_target>> : t_derivable<t_bears<xemmaix::nata::t_overlay<T_target>, t_type_of<xemmaix::nata::t_proxy>>>
{
	using t_library = typename T_target::t_library;
	using t_overlay = xemmaix::nata::t_overlay<T_target>;

	static void f_define(t_library* a_library)
	{
		t_define{a_library}
		(L"paint"sv, t_member<void(*)(t_overlay&, size_t, size_t, bool), [](auto a_this, auto a_p, auto a_n, auto a_on)
		{
			size_t n = a_this.v_view.v_text.f_size();
			if ((a_p > n)) f_throw(L"out of range."sv);
			a_this.v_overlay->f_paint(a_p, {{a_on, std::min(a_n, n - a_p)}});
		}>())
		.template f_derive<t_overlay, xemmaix::nata::t_proxy>();
	}

	using t_type_of::t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n)
	{
		return t_construct<xemmaix::nata::t_view<T_target>&, const typename T_target::t_attribute&>::template t_bind<t_overlay>::f_do(this, a_stack, a_n);
	}
};

template<typename T_target>
struct t_type_of<xemmaix::nata::t_overlay_iterator<T_target>> : t_derivable<t_bears<xemmaix::nata::t_overlay_iterator<T_target>, t_type_of<xemmaix::nata::t_proxy>>>
{
	using t_library = typename T_target::t_library;
	using t_overlay_iterator = xemmaix::nata::t_overlay_iterator<T_target>;

	static void f_define(t_library* a_library)
	{
		t_define{a_library}
		(L"next"sv, t_member<t_object*(*)(t_library*, t_overlay_iterator&), [](auto a_library, auto a_this)
		{
			auto& i = a_this.v_i;
			if (!a_this.v_overlay.f_valid() || i == a_this.v_overlay.v_overlay->f_end()) return static_cast<t_object*>(nullptr);
			auto p = f_new_value(a_library->f_type_overlay_value(), i->v_x, i.f_index().v_i1, i.f_delta().v_i1);
			++i;
			return p;
		}>())
		.template f_derive<t_overlay_iterator, xemmaix::nata::t_proxy>();
	}

	using t_type_of::t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n)
	{
		return t_construct<xemmaix::nata::t_overlay<T_target>&>::template t_bind<t_overlay_iterator>::f_do(this, a_stack, a_n);
	}
};

}

#endif
