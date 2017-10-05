#ifndef NATA__SPANS_H
#define NATA__SPANS_H

#include <deque>
#include <jumoku/array.h>

namespace nata
{

template<typename T_value, size_t A_leaf = 4096, size_t A_branch = 4096>
struct t_spans
{
	struct t_span
	{
		T_value v_x;
		size_t v_n;

		bool operator==(const t_span& a_x) const
		{
			return v_x == a_x.v_x && v_n == a_x.v_n;
		}
	};
	template<typename T>
	struct t_index
	{
		T v_i0;
		T v_i1;

		template<typename U>
		operator t_index<U>() const
		{
			return {U(v_i0), U(v_i1)};
		}
		t_index<int> operator-() const
		{
			return {-int(v_i0), -int(v_i1)};
		}
		bool operator==(const t_index& a_x) const
		{
			return v_i0 == a_x.v_i0 && v_i1 == a_x.v_i1;
		}
		template<typename U>
		t_index& operator+=(const t_index<U>& a_x)
		{
			v_i0 += a_x.v_i0;
			v_i1 += a_x.v_i1;
			return *this;
		}
		template<typename U>
		t_index& operator-=(const t_index<U>& a_x)
		{
			v_i0 -= a_x.v_i0;
			v_i1 -= a_x.v_i1;
			return *this;
		}
		template<typename U>
		t_index operator+(const t_index<U>& a_x) const
		{
			return {v_i0 + T(a_x.v_i0), v_i1 + T(a_x.v_i1)};
		}
		template<typename U>
		t_index operator-(const t_index<U>& a_x) const
		{
			return {v_i0 - T(a_x.v_i0), v_i1 - T(a_x.v_i1)};
		}
	};

protected:
	struct t_traits
	{
		typedef t_spans::t_index<size_t> t_index;
		typedef t_spans::t_index<int> t_delta;
		struct t_default
		{
			constexpr size_t operator()(const t_index& a_index) const
			{
				return a_index.v_i0;
			}
		};

		template<typename T>
		static constexpr t_index f_index(size_t a_n, const T& a_value)
		{
			return {a_n, a_value.v_n};
		}
		template<typename T>
		static constexpr void f_add(T& a_value, const t_delta& a_index)
		{
			a_value.v_n += a_index.v_i1;
		}
		template<typename T>
		static constexpr T f_get(T* a_base, T* a_p)
		{
			return {a_p->v_x, f_delta(a_base, a_p).v_i1};
		}
		template<typename T>
		static constexpr t_index f_delta(T* a_base, T* a_p)
		{
			return {1, a_p > a_base ? a_p->v_n - a_p[-1].v_n : a_p->v_n};
		}
	};
	typedef jumoku::t_array<t_span, A_leaf, A_branch, t_traits> t_array;

	t_array v_array;

public:
	typedef typename t_array::t_constant_iterator t_iterator;

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
			return a_index.v_i1;
		});
	}
};

template<typename T_value, size_t A_leaf = 4096, size_t A_branch = 4096>
struct t_stretches : t_spans<T_value, A_leaf, A_branch>
{
	typedef typename t_spans<T_value, A_leaf, A_branch>::t_span t_span;
	typedef typename t_spans<T_value, A_leaf, A_branch>::t_iterator t_iterator;

	t_iterator f_replace(size_t a_p, size_t a_n, std::deque<t_span>&& a_xs)
	{
		auto i = this->f_at_in_text(a_p);
		if (a_n <= 0 && a_xs.empty()) return i;
		size_t n = a_p - i.f_index().v_i1;
		if (n > 0) {
			if (!a_xs.empty() && a_xs.front().v_x == i->v_x)
				a_xs.front().v_n += n;
			else
				a_xs.push_front({i->v_x, n});
		} else if (!a_xs.empty() && i != this->f_begin()) {
			auto j = i;
			if (a_xs.front().v_x == (--j)->v_x) {
				a_xs.front().v_n += j.f_delta().v_i1;
				i = j;
			}
		}
		size_t p = a_p + a_n;
		auto j = this->f_at_in_text(p);
		if (j.f_index().v_i1 < p) {
			auto x = j->v_x;
			size_t n = (++j).f_index().v_i1 - p;
			if (!a_xs.empty() && a_xs.back().v_x == x)
				a_xs.back().v_n += n;
			else
				a_xs.push_back({x, n});
		} else if (j != this->f_end()) {
			if (!a_xs.empty()) {
				if (a_xs.back().v_x == j->v_x) {
					a_xs.back().v_n += j.f_delta().v_i1;
					++j;
				}
			} else if (i != this->f_begin()) {
				auto k = i;
				if ((--k)->v_x == j->v_x) {
					a_xs.push_back({k->v_x, k.f_delta().v_i1 + j.f_delta().v_i1});
					i = k;
					++j;
				}
			}
		}
		i = this->v_array.f_insert(this->v_array.f_erase(i, j), a_xs.begin(), a_xs.end());
		if (i.f_index().v_i1 + i.f_delta().v_i1 == a_p) ++i;
		return i;
	}
};

}

#endif
