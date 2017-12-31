#ifndef NATA__WIDGET_H
#define NATA__WIDGET_H

#include "stretches.h"
#include "signal.h"
#include <vector>

namespace nata
{

#if 0
constexpr size_t c_region_chunk = 8;
#else
constexpr size_t c_region_chunk = 5;
#endif

template<typename T_rows>
class t_widget
{
	const T_rows& v_rows;
	t_stretches<bool, c_region_chunk, c_region_chunk> v_region;

	void f_dirty(size_t a_y, size_t a_height, bool a_dirty)
	{
		v_region.f_replace(a_y, a_height, {{a_dirty, a_height}});
	}
	void f_scroll(size_t a_y, int a_delta)
	{
		size_t height = f_height();
		size_t range = height - a_y;
		if (a_delta >= range) {
			f_dirty(a_y, range, true);
			return;
		}
		if (a_delta < 0) {
			v_region.f_replace(height + a_delta, -a_delta, {});
			v_region.f_replace(a_y, 0, {{true, size_t(-a_delta)}});
		} else {
			v_region.f_replace(a_y, a_delta, {});
			v_region.f_replace(height - a_delta, 0, {{true, size_t(a_delta)}});
		}
		v_rows.v_target.f_scroll(a_y, height, a_delta);
	}
	void f_adjust(size_t a_y, size_t a_h0, size_t a_h1)
	{
		int y = a_y - v_top;
		int height = f_height();
		if (y >= height) return;
		int b0 = y + int(a_h0);
		int b1 = y + int(a_h1);
		if (std::max(b0, b1) >= height) {
			if (y < 0) y = 0;
			f_dirty(y, height - y, true);
		} else if (b0 <= 0) {
			v_top += b1 - b0;
		} else if (b1 <= 0) {
			f_scroll(0, b0);
			v_top = b1;
		} else {
			if (y < 0) y = 0;
			int b = std::min(b0, b1);
			f_dirty(y, b - y, true);
			f_scroll(b, b0 - b1);
		}
		f_top__(v_top);
	}
	nata::t_slot<size_t, size_t, size_t, size_t, size_t, size_t> v_replaced = [this](auto a_p, auto a_n0, auto a_n1, auto a_y, auto a_h0, auto a_h1)
	{
		f_adjust(a_y, a_h0, a_h1);
		size_t& p = std::get<0>(v_position);
		if (p < a_p) return;
		if (p > a_p || a_n0 <= 0) p = (p < a_p + a_n0 ? a_p : p - a_n0) + a_n1;
		f_from_position(v_row.f_index().v_y < a_y + a_h0);
	};
	nata::t_slot<size_t, size_t, size_t, size_t, size_t> v_painted = [this](auto, auto, auto a_y, auto a_h0, auto a_h1)
	{
		f_adjust(a_y, a_h0, a_h1);
		f_from_position();
	};

public:
	size_t v_top = 0;
	std::tuple<size_t, size_t, size_t> v_position{0, 0, 0};
	typename T_rows::template t_index<size_t> v_line{0, 0, 0, 0, 0};
	size_t v_target = 0;
	typename T_rows::t_iterator v_row;

	t_widget(T_rows& a_rows, size_t a_height) : v_rows(a_rows)
	{
		v_region.f_replace(0, 0, {{true, a_height}});
		a_rows.v_replaced >> v_replaced;
		a_rows.v_painted >> v_painted;
	}
	size_t f_height() const
	{
		return v_region.f_size().v_i1;
	}
	size_t f_range() const
	{
		size_t height = f_height();
		return std::max(v_rows.f_size().v_i, height) - height;
	}
	template<typename T_graphics>
	void f_render(T_graphics& a_target)
	{
		auto row = v_rows.f_at_in_y(v_top);
		size_t p = row.f_index().v_text;
		auto text = v_rows.v_tokens.v_text.f_at(p);
		std::vector<typename T_rows::t_foldings::t_iterator> folding;
		size_t q = v_rows.f_leaf_at_in_text(p, folding, true);
		size_t fd = folding.back().f_delta().v_i1 - q;
		auto token = v_rows.v_tokens.f_at_in_text(p);
		size_t td = token.f_index().v_i1 + token.f_delta().v_i1 - p;
		auto next = [&](size_t d)
		{
			td -= d;
			if (td <= 0) td = (++token).f_delta().v_i1;
			fd -= d;
			if (fd <= 0) {
				v_rows.f_next_leaf(folding);
				fd = folding.back().f_delta().v_i1;
			}
		};
		auto dirty = v_region.f_begin();
		auto dend = v_region.f_end();
		if (dirty != dend && !dirty->v_x) ++dirty;
		size_t y = 0;
		while (dirty != dend) {
			auto rd = row.f_delta();
			if (row->v_tail) --rd.v_text;
			auto run = [&](auto folded, auto puts)
			{
				while (rd.v_text > 0) {
					if (folding.back()->v_x) {
						size_t p = text.f_index();
						do {
							folded();
							size_t d = folding.back().f_delta().v_i1;
							p += d;
							rd.v_text -= d;
							v_rows.f_next_leaf(folding);
						} while (rd.v_text > 0 && folding.back()->v_x);
						fd = folding.back().f_delta().v_i1;
						token = v_rows.v_tokens.f_at_in_text(p);
						td = token.f_index().v_i1 + token.f_delta().v_i1 - p;
						text = v_rows.v_tokens.v_text.f_at(p);
					}
					if (rd.v_text <= 0) break;
					size_t d = std::min(fd, std::min(td, rd.v_text));
					puts(d);
					next(d);
					rd.v_text -= d;
				}
				y += rd.v_y;
			};
			if (y + rd.v_y > dirty.f_index().v_i1) {
				a_target.f_move(y);
				run([&]
				{
					a_target.f_folded();
				}, [&](size_t d)
				{
					a_target.f_attribute(token->v_x);
					for (size_t i = 0; i < d; ++i) {
						wchar_t c = *text;
						if (c == L'\t')
							a_target.f_tab();
						else
							a_target.f_put(c);
						++text;
					}
				});
				if (dirty.f_index().v_i1 + dirty.f_delta().v_i1 <= y) {
					do ++dirty; while (dirty != dend && dirty.f_index().v_i1 + dirty.f_delta().v_i1 <= y);
					if (dirty != dend && !dirty->v_x) ++dirty;
				}
				if (row->v_tail) {
					if (++row == v_rows.f_end()) {
						a_target.f_eof();
						break;
					}
					a_target.f_eol();
					++text;
					next(1);
				} else {
					++row;
					a_target.f_wrap();
				}
			} else {
				run([&] {}, [&](size_t d)
				{
					for (size_t i = 0; i < d; ++i) ++text;
				});
				if (row->v_tail) {
					if (++row == v_rows.f_end()) break;
					++text;
					next(1);
				} else {
					++row;
				}
			}
		}
		if (y < dirty.f_index().v_i1) y = dirty.f_index().v_i1;
		while (dirty != dend) {
			for (size_t b = dirty.f_index().v_i1 + dirty.f_delta().v_i1; y < b; ++y) a_target.f_empty(y);
			if (++dirty == dend) break;
			y = (++dirty).f_index().v_i1;
		}
		f_dirty(0, f_height(), false);
	}
	void f_top__(size_t a_value)
	{
		size_t n = f_range();
		if (a_value > n) a_value = n;
		if (a_value == v_top) return;
		f_scroll(0, int(a_value) - int(v_top));
		v_top = a_value;
	}
	void f_into_view(size_t a_y, size_t a_height)
	{
		size_t top = v_top;
		size_t bottom = a_y + a_height;
		size_t height = f_height();
		if (bottom > top + height) top = bottom - height;
		if (a_y < top) top = a_y;
		f_top__(top);
	}
	void f_into_view(const decltype(v_rows.f_begin())& a_row)
	{
		f_into_view(a_row.f_index().v_y, a_row.f_delta().v_y);
	}
	void f_from_line()
	{
		v_line = v_rows.f_at_in_line(v_line.v_line).f_index();
		size_t ax = v_line.v_x + v_target;
		v_row = v_rows.f_at_in_line(v_line.v_line + 1);
		if (ax < v_row.f_index().v_x)
			v_row = v_rows.f_at_in_x(ax);
		else
			--v_row;
		v_position = v_rows.f_each_x(v_row, [&](size_t p, size_t x, size_t width)
		{
			return x + width <= ax;
		});
	}
	void f_from_position(bool a_retarget = false)
	{
		v_row = v_rows.f_at_in_text(std::get<0>(v_position));
		v_line = v_rows.f_at_in_line(v_row.f_index().v_line + v_row.f_delta().v_line - 1).f_index();
		v_position = v_rows.f_each_x(v_row, [&](size_t p, size_t x, size_t width)
		{
			return p < std::get<0>(v_position);
		});
		size_t tx = std::get<1>(v_position) - v_line.v_x;
		if (a_retarget || v_target < tx || std::get<0>(v_position) < v_rows.f_at_in_line(v_line.v_line + 1).f_index().v_text - 1 && v_target >= tx + std::get<2>(v_position)) v_target = tx;
	}
};

}

#endif
