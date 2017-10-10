#ifndef NATA__VIEW_H
#define NATA__VIEW_H

#include "model.h"

#include <numeric>

namespace nata
{

template<typename T_text, typename T_value, size_t A_leaf = 4096, size_t A_branch = 4096>
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
		v_text_replaced(0, 0, v_text.f_size());
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

template<typename T_tokens, typename T_target, size_t A_leaf = 4096, size_t A_branch = 4096>
struct t_rows
{
	struct t_row
	{
		bool v_head;
		bool v_tail;
		size_t v_text;
		size_t v_width;
		size_t v_ascent;
		size_t v_height;

		bool operator==(const t_row& a_x) const
		{
			return v_head == a_x.v_head && v_tail == a_x.v_tail && v_text == a_x.v_text && v_width == a_x.v_width && v_ascent == a_x.v_ascent && v_height == a_x.v_height;
		}
	};
	template<typename T>
	struct t_index
	{
		T v_i;
		T v_text;
		T v_x;
		T v_y;

		template<typename U>
		operator t_index<U>() const
		{
			return {U(v_i), U(v_text), U(v_x), U(v_y)};
		}
		t_index<int> operator-() const
		{
			return {-int(v_i), -int(v_text), -int(v_x), -int(v_y)};
		}
		bool operator==(const t_index& a_x) const
		{
			return v_i == a_x.v_i && v_text == a_x.v_text && v_x == a_x.v_x && v_y == a_x.v_y;
		}
		template<typename U>
		t_index& operator+=(const t_index<U>& a_x)
		{
			v_i += a_x.v_i;
			v_text += a_x.v_text;
			v_x += a_x.v_x;
			v_y += a_x.v_y;
			return *this;
		}
		template<typename U>
		t_index& operator-=(const t_index<U>& a_x)
		{
			v_i -= a_x.v_i;
			v_text -= a_x.v_text;
			v_x -= a_x.v_x;
			v_y -= a_x.v_y;
			return *this;
		}
		template<typename U>
		t_index operator+(const t_index<U>& a_x) const
		{
			return {v_i + T(a_x.v_i), v_text + T(a_x.v_text), v_x + T(a_x.v_x), v_y + T(a_x.v_y)};
		}
		template<typename U>
		t_index operator-(const t_index<U>& a_x) const
		{
			return {v_i - T(a_x.v_i), v_text - T(a_x.v_text), v_x - T(a_x.v_x), v_y - T(a_x.v_y)};
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
			return {a_n, a_value.v_text, a_value.v_width, a_value.v_height};
		}
		template<typename T>
		static constexpr void f_add(T& a_value, const t_delta& a_index)
		{
			a_value.v_text += a_index.v_text;
			a_value.v_width += a_index.v_x;
			a_value.v_height += a_index.v_y;
		}
		template<typename T>
		static constexpr T f_get(T* a_base, T* a_p)
		{
			auto d = f_delta(a_base, a_p);
			return {a_p->v_head, a_p->v_tail, d.v_text, d.v_x, a_p->v_ascent, d.v_y};
		}
		template<typename T>
		static constexpr t_index f_delta(T* a_base, T* a_p)
		{
			return a_p > a_base ? t_index{1, a_p->v_text - a_p[-1].v_text, a_p->v_width - a_p[-1].v_width, a_p->v_height - a_p[-1].v_height} : t_index{1, a_p->v_text, a_p->v_width, a_p->v_height};
		}
	};
	typedef jumoku::t_array<t_row, A_leaf, A_branch, t_traits> t_array;

	t_array v_array;

	std::tuple<size_t, size_t, size_t> f_replace(size_t a_p, size_t a_n0, size_t a_n1)
	{
		auto i = f_at_in_text(a_p);
		if (i.f_index().v_text >= a_p && !i->v_head) --i;
		bool head = i->v_head;
		size_t p = a_p + a_n0;
		auto j = f_at_in_text(p);
		do ++j; while (j != f_end() && !j->v_head);
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
			i = v_array.f_insert(i, t_row{head, next, text, width, ascent, ascent + descent});
			++i;
			head = next;
			text = width = ascent = descent = 0;
		};
		auto advance = [&](const auto& size)
		{
			++text;
			width += std::get<0>(size);
			if (std::get<1>(size) > ascent) ascent = std::get<1>(size);
			if (std::get<2>(size) > descent) descent = std::get<2>(size);
		};
		auto token = v_tokens.f_at_in_text(i.f_index().v_text);
		size_t delta = token.f_index().v_i1 + token.f_delta().v_i1 - i.f_index().v_text;
		auto last = v_tokens.v_text.f_at(a_p + a_n1);
		for (auto first = v_tokens.v_text.f_at(i.f_index().v_text); first != last; ++first) {
			wchar_t c = *first;
			switch (c) {
			case L'\t':
				{
					auto size = v_target.f_tab(width, token->v_x);
					if (overflow(size)) {
						wrap(false);
						size = v_target.f_tab(width, token->v_x);
					}
					advance(size);
				}
				break;
			case L'\n':
				{
					auto size = v_target.f_eol(token->v_x);
					if (overflow(size)) wrap(false);
					advance(size);
					wrap(true);
				}
				break;
			default:
				{
					auto size = v_target.f_size(c, token->v_x);
					if (overflow(size)) wrap(false);
					advance(size);
				}
			}
			if (--delta <= 0) delta = (++token).f_delta().v_i1;
		}
		if (i == f_end()) {
			auto size = v_target.f_eof();
			if (overflow(size)) wrap(false);
			advance(size);
			i = v_array.f_insert(i, t_row{head, true, text, width, ascent, ascent + descent});
			++i;
		}
		return {y, bottom - y, i.f_index().v_y - y};
	};
	t_slot<size_t, size_t, size_t> v_tokens_replaced{[this](auto a_p, auto a_n0, auto a_n1)
	{
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
		auto size = v_target.f_eof();
		v_array.f_insert(f_end(), t_row{true, true, 1, std::get<0>(size), std::get<1>(size), std::get<1>(size) + std::get<2>(size)});
		v_tokens.v_replaced >> v_tokens_replaced;
		v_tokens.v_painted >> v_tokens_painted;
		v_target.v_resized >> v_target_resized;
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
	template<typename T>
	std::tuple<size_t, size_t, size_t> f_each_x(t_iterator a_i, T a_do) const
	{
		size_t p = a_i.f_index().v_text;
		size_t x = a_i.f_index().v_x;
		auto token = v_tokens.f_at_in_text(p);
		size_t delta = token.f_index().v_i1 + token.f_delta().v_i1 - p;
		size_t q = p + a_i.f_delta().v_text;
		auto last = v_tokens.v_text.f_at(a_i->v_tail ? q - 1 : q);
		for (auto first = v_tokens.v_text.f_at(p); first != last; ++first) {
			wchar_t c = *first;
			size_t width = std::get<0>(c == L'\t' ? v_target.f_tab(x - a_i.f_index().v_x, token->v_x) : v_target.f_size(c, token->v_x));
			if (!a_do(p, x, width)) return std::make_tuple(p, x, width);
			++p;
			x += width;
			if (--delta <= 0) delta = (++token).f_delta().v_i1;
		}
		return std::make_tuple(p, x, a_i->v_tail ? std::get<0>(p < v_tokens.v_text.f_size() ? v_target.f_eol(token->v_x) : v_target.f_eof()) : 0);
	}
};

}

#endif
