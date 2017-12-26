#ifndef NATA__VIEW_H
#define NATA__VIEW_H

#include "model.h"
#include "stretches.h"
#include "nested.h"
#include <numeric>

namespace nata
{

template<typename T_text, typename T_value, size_t A_leaf = 256, size_t A_branch = 256>
class t_tokens : t_stretches<T_value, A_leaf, A_branch>
{
	typedef nata::t_stretches<T_value, A_leaf, A_branch> t_stretches;

	t_slot<size_t, size_t, size_t> v_text_replaced{[this](auto a_p, auto a_n0, auto a_n1)
	{
		std::deque<typename t_stretches::t_span> xs;
		if (a_n1 > 0) xs.push_back({{}, a_n1});
		this->f_replace(a_p, a_n0, std::move(xs));
		v_replaced(a_p, a_n0, a_n1);
	}};

public:
	typedef typename t_stretches::t_span t_span;
	typedef typename t_stretches::t_iterator t_iterator;

	T_text& v_text;
	t_signal<size_t, size_t, size_t> v_replaced;
	t_signal<size_t, size_t> v_painted;

	t_tokens(T_text& a_text) : v_text(a_text)
	{
		v_text.v_replaced >> v_text_replaced;
		this->f_replace(0, 0, {{{}, v_text.f_size() + 1}});
	}
	using t_stretches::f_size;
	using t_stretches::f_begin;
	using t_stretches::f_end;
	using t_stretches::f_at;
	using t_stretches::f_at_in_text;
	t_iterator f_paint(size_t a_p, std::deque<t_span>&& a_xs)
	{
		size_t n = std::accumulate(a_xs.begin(), a_xs.end(), 0, [](size_t n, const auto& x)
		{
			return n + x.v_n;
		});
		auto i = this->f_replace(a_p, n, std::move(a_xs));
		v_painted(a_p, n);
		return i;
	}
};

template<size_t A_leaf = 16, size_t A_branch = 16>
struct t_foldable
{
	t_nested<t_foldable, A_leaf, A_branch> v_nested;
	bool v_folded = false;
};

template<typename T_tokens, typename T_target, typename T_foldable = t_foldable<>, size_t A_leaf = 256, size_t A_branch = 256>
struct t_rows
{
	typedef decltype(T_foldable::v_nested) t_foldings;
	struct t_row
	{
		size_t v_line;
		bool v_tail;
		size_t v_text;
		size_t v_width;
		size_t v_ascent;
		size_t v_height;

		bool operator==(const t_row& a_x) const
		{
			return v_line == a_x.v_line && v_tail == a_x.v_tail && v_text == a_x.v_text && v_width == a_x.v_width && v_ascent == a_x.v_ascent && v_height == a_x.v_height;
		}
	};
	template<typename T>
	struct t_index
	{
		T v_i;
		T v_line;
		T v_text;
		T v_x;
		T v_y;

		template<typename U>
		operator t_index<U>() const
		{
			return {U(v_i), U(v_line), U(v_text), U(v_x), U(v_y)};
		}
		t_index<int> operator-() const
		{
			return {-int(v_i), -int(v_line), -int(v_text), -int(v_x), -int(v_y)};
		}
		bool operator==(const t_index& a_x) const
		{
			return v_i == a_x.v_i && v_line == a_x.v_line && v_text == a_x.v_text && v_x == a_x.v_x && v_y == a_x.v_y;
		}
		template<typename U>
		t_index& operator+=(const t_index<U>& a_x)
		{
			v_i += a_x.v_i;
			v_line += a_x.v_line;
			v_text += a_x.v_text;
			v_x += a_x.v_x;
			v_y += a_x.v_y;
			return *this;
		}
		template<typename U>
		t_index& operator-=(const t_index<U>& a_x)
		{
			v_i -= a_x.v_i;
			v_line -= a_x.v_line;
			v_text -= a_x.v_text;
			v_x -= a_x.v_x;
			v_y -= a_x.v_y;
			return *this;
		}
		template<typename U>
		t_index operator+(const t_index<U>& a_x) const
		{
			return {v_i + T(a_x.v_i), v_line + T(a_x.v_line), v_text + T(a_x.v_text), v_x + T(a_x.v_x), v_y + T(a_x.v_y)};
		}
		template<typename U>
		t_index operator-(const t_index<U>& a_x) const
		{
			return {v_i - T(a_x.v_i), v_line - T(a_x.v_line), v_text - T(a_x.v_text), v_x - T(a_x.v_x), v_y - T(a_x.v_y)};
		}
	};

private:
	struct t_traits
	{
		typedef t_rows::t_index<size_t> t_index;
		typedef t_rows::t_index<int> t_delta;
		struct t_default
		{
			constexpr size_t operator()(const t_index& a_index) const
			{
				return a_index.v_i;
			}
		};

		template<typename T>
		static constexpr t_index f_index(size_t a_n, const T& a_value)
		{
			return {a_n, a_value.v_line, a_value.v_text, a_value.v_width, a_value.v_height};
		}
		template<typename T>
		static constexpr void f_add(T& a_value, const t_delta& a_index)
		{
			a_value.v_line += a_index.v_line;
			a_value.v_text += a_index.v_text;
			a_value.v_width += a_index.v_x;
			a_value.v_height += a_index.v_y;
		}
		template<typename T>
		static constexpr T f_get(T* a_base, T* a_p)
		{
			auto d = f_delta(a_base, a_p);
			return {d.v_line, a_p->v_tail, d.v_text, d.v_x, a_p->v_ascent, d.v_y};
		}
		template<typename T>
		static constexpr t_index f_delta(T* a_base, T* a_p)
		{
			return a_p > a_base ? t_index{1, a_p->v_line - a_p[-1].v_line, a_p->v_text - a_p[-1].v_text, a_p->v_width - a_p[-1].v_width, a_p->v_height - a_p[-1].v_height} : t_index{1, a_p->v_line, a_p->v_text, a_p->v_width, a_p->v_height};
		}
	};
	typedef jumoku::t_array<t_row, A_leaf, A_branch, t_traits> t_array;

	template<bool A_visible>
	static constexpr bool f_enter(const std::shared_ptr<T_foldable>& a_x)
	{
		return !A_visible || !a_x->v_folded;
	}

	t_array v_array;
	t_foldings v_foldings;

	std::tuple<size_t, size_t, size_t> f_replace(size_t a_p, size_t a_n0, size_t a_n1)
	{
		auto i = f_at_in_text(a_p);
		bool head = i.f_delta().v_line > 0;
		if (i.f_index().v_text >= a_p && !head) head = (--i).f_delta().v_line > 0;
		size_t p = a_p + a_n0;
		auto j = f_at_in_text(p);
		do ++j; while (j != f_end() && j.f_delta().v_line <= 0);
		a_n1 += j.f_index().v_text - p;
		if (j == f_end()) --a_n1;
		size_t bottom = j.f_index().v_y;
		i = v_array.f_erase(i, j);
		size_t y = i.f_index().v_y;
		size_t text = 0;
		size_t width = 0;
		size_t ascent = 0;
		size_t descent = 0;
		auto overflow = [&](const auto& size)
		{
			return width + std::get<0>(size) > v_target.f_width() && text > 0;
		};
		auto wrap = [&](bool next)
		{
			i = v_array.f_insert(i, t_row{head ? 1u : 0u, next, text, width, ascent, ascent + descent});
			++i;
			head = next;
			text = width = ascent = descent = 0;
		};
		auto advance = [&](size_t n, const auto& size)
		{
			text += n;
			width += std::get<0>(size);
			if (std::get<1>(size) > ascent) ascent = std::get<1>(size);
			if (std::get<2>(size) > descent) descent = std::get<2>(size);
		};
		auto cell = [&](size_t n, const auto& size)
		{
			if (overflow(size)) wrap(false);
			advance(n, size);
		};
		p = i.f_index().v_text;
		std::vector<typename t_foldings::t_iterator> folding;
		size_t q = f_leaf_at_in_text(p, folding, true);
		size_t fd = folding.back().f_delta().v_i1 - q;
		auto token = v_tokens.f_at_in_text(p);
		size_t td = token.f_index().v_i1 + token.f_delta().v_i1 - p;
		auto first = v_tokens.v_text.f_at(p);
		auto last = v_tokens.v_text.f_at(a_p + a_n1);
		while (first != last) {
			if (folding.back()->v_x) {
				size_t p = first.f_index();
				do {
					size_t d = folding.back().f_delta().v_i1;
					cell(d, v_target.f_folded());
					p += d;
					f_next_leaf(folding);
				} while (folding.back()->v_x);
				fd = folding.back().f_delta().v_i1;
				token = v_tokens.f_at_in_text(p);
				td = token.f_index().v_i1 + token.f_delta().v_i1 - p;
				first = v_tokens.v_text.f_at(p);
			}
			while (true) {
				wchar_t c = *first;
				switch (c) {
				case L'\t':
					{
						auto size = v_target.f_tab(width, token->v_x);
						if (overflow(size)) {
							wrap(false);
							size = v_target.f_tab(width, token->v_x);
						}
						advance(1, size);
					}
					break;
				case L'\n':
					cell(1, v_target.f_eol(token->v_x));
					wrap(true);
					break;
				default:
					cell(1, v_target.f_size(c, token->v_x));
				}
				if (++first == last) break;
				if (--td <= 0) td = (++token).f_delta().v_i1;
				if (--fd <= 0) {
					f_next_leaf(folding);
					fd = folding.back().f_delta().v_i1;
					break;
				}
			}
		}
		if (i == f_end()) {
			cell(1, v_target.f_eof());
			i = v_array.f_insert(i, t_row{head ? 1u : 0u, true, text, width, ascent, ascent + descent});
			++i;
		}
		return {y, bottom - y, i.f_index().v_y - y};
	};
	t_slot<size_t, size_t, size_t> v_tokens_replaced{[this](auto a_p, auto a_n0, auto a_n1)
	{
		v_foldings.f_replace(a_p, a_n0, {{a_n1}});
		auto x = f_replace(a_p, a_n0, a_n1);
		v_replaced(a_p, a_n0, a_n1, std::get<0>(x), std::get<1>(x), std::get<2>(x));
	}};
	t_slot<size_t, size_t> v_tokens_painted{[this](auto a_p, auto a_n)
	{
		auto x = f_replace(a_p, a_n, a_n);
		v_painted(a_p, a_n, std::get<0>(x), std::get<1>(x), std::get<2>(x));
	}};
	t_slot<> v_target_resized{[this]
	{
		v_tokens_painted(0, v_tokens.v_text.f_size());
	}};

public:
	typedef typename t_array::t_constant_iterator t_iterator;

	t_signal<size_t, size_t, size_t, size_t, size_t, size_t> v_replaced;
	t_signal<size_t, size_t, size_t, size_t, size_t> v_painted;

	T_tokens& v_tokens;
	T_target& v_target;

	t_rows(T_tokens& a_tokens, T_target& a_target) : v_tokens(a_tokens), v_target(a_target)
	{
		v_tokens.v_replaced >> v_tokens_replaced;
		v_tokens.v_painted >> v_tokens_painted;
		v_target.v_resized >> v_target_resized;
		auto size = v_target.f_eof();
		v_array.f_insert(f_end(), t_row{1, true, 1, std::get<0>(size), std::get<1>(size), std::get<1>(size) + std::get<2>(size)});
		v_foldings.f_replace(0, 0, {{v_tokens.v_text.f_size() + 1}});
		f_replace(0, 0, v_tokens.v_text.f_size());
	}
	t_index<size_t> f_size() const
	{
		return v_array.f_size();
	}
	t_iterator f_begin() const
	{
		return v_array.f_begin();
	}
	t_iterator f_end() const
	{
		return v_array.f_end();
	}
	t_iterator f_at(size_t a_p) const
	{
		return v_array.f_at(a_p);
	}
	t_iterator f_at_in_line(size_t a_p) const
	{
		return v_array.f_at(a_p, [](const auto& a_index)
		{
			return a_index.v_line;
		});
	}
	t_iterator f_at_in_text(size_t a_p) const
	{
		return v_array.f_at(a_p, [](const auto& a_index)
		{
			return a_index.v_text;
		});
	}
	t_iterator f_at_in_x(size_t a_p) const
	{
		return v_array.f_at(a_p, [](const auto& a_index)
		{
			return a_index.v_x;
		});
	}
	t_iterator f_at_in_y(size_t a_p) const
	{
		return v_array.f_at(a_p, [](const auto& a_index)
		{
			return a_index.v_y;
		});
	}
	const t_foldings& f_foldings() const
	{
		return v_foldings;
	}
	size_t f_folding_at_in_text(size_t a_p, std::vector<typename t_foldings::t_iterator>& a_path, bool a_visible = false) const
	{
		return v_foldings.f_path_at_in_text(a_p, a_path, a_visible ? f_enter<true> : f_enter<false>);
	}
	size_t f_leaf_at_in_text(size_t a_p, std::vector<typename t_foldings::t_iterator>& a_path, bool a_visible = false) const
	{
		return v_foldings.f_leaf_at_in_text(a_p, a_path, a_visible ? f_enter<true> : f_enter<false>);
	}
	void f_next_leaf(std::vector<typename t_foldings::t_iterator>& a_path) const
	{
		v_foldings.f_next(a_path);
		v_foldings.f_leaf(a_path, f_enter<true>);
	}
	typename t_foldings::t_iterator f_foldable(size_t a_p, std::deque<typename t_foldings::t_span>&& a_xs)
	{
		size_t n = std::accumulate(a_xs.begin(), a_xs.end(), 0, [](size_t n, const auto& x)
		{
			return n + x.v_n;
		});
		auto i = v_foldings.f_replace(a_p, n, std::move(a_xs));
		v_tokens_painted(a_p, n);
		return i;
	}
	void f_folded(size_t a_p, bool v_folded)
	{
		std::vector<typename t_foldings::t_iterator> path;
		f_folding_at_in_text(a_p, path);
		auto& i = path.back();
		if (!i->v_x) return;
		i->v_x->v_folded = v_folded;
		v_tokens_painted(a_p, i.f_delta().v_i1);
	}
	template<typename T>
	std::tuple<size_t, size_t, size_t> f_each_x(t_iterator a_i, T a_do) const
	{
		size_t p = a_i.f_index().v_text;
		size_t x = a_i.f_index().v_x;
		std::vector<typename t_foldings::t_iterator> folding;
		size_t q = f_leaf_at_in_text(p, folding, true);
		size_t fd = folding.back().f_delta().v_i1 - q;
		auto token = v_tokens.f_at_in_text(p);
		size_t td = token.f_index().v_i1 + token.f_delta().v_i1 - p;
		auto first = v_tokens.v_text.f_at(p);
		p += a_i.f_delta().v_text;
		auto last = v_tokens.v_text.f_at(a_i->v_tail ? p - 1 : p);
		while (first != last) {
			if (folding.back()->v_x) {
				size_t p = first.f_index();
				do {
					size_t width = std::get<0>(v_target.f_folded());
					if (!a_do(p, x, width)) return {p, x, width};
					p += folding.back().f_delta().v_i1;
					x += width;
					f_next_leaf(folding);
				} while (folding.back()->v_x);
				fd = folding.back().f_delta().v_i1;
				token = v_tokens.f_at_in_text(p);
				td = token.f_index().v_i1 + token.f_delta().v_i1 - p;
				first = v_tokens.v_text.f_at(p);
			}
			while (true) {
				wchar_t c = *first;
				size_t width = std::get<0>(c == L'\t' ? v_target.f_tab(x - a_i.f_index().v_x, token->v_x) : v_target.f_size(c, token->v_x));
				if (!a_do(first.f_index(), x, width)) return {first.f_index(), x, width};
				x += width;
				if (--td <= 0) td = (++token).f_delta().v_i1;
				if (++first == last) break;
				if (--fd <= 0) {
					f_next_leaf(folding);
					fd = folding.back().f_delta().v_i1;
					break;
				}
			}
		}
		p = first.f_index();
		return {p, x, a_i->v_tail ? std::get<0>(p < v_tokens.v_text.f_size() ? v_target.f_eol(token->v_x) : v_target.f_eof()) : 0};
	}
};

}

#endif
