#ifndef NATA__SPANS_H
#define NATA__SPANS_H

#include <jumoku/array.h>

namespace nata
{

template<typename T_value>
struct t_span
{
	T_value v_x;
	size_t v_n;

	bool operator==(const t_span& a_x) const
	{
		return v_x == a_x.v_x && v_n == a_x.v_n;
	}
	t_span f_get(size_t a_n) const
	{
		return {v_x, a_n};
	}
};

template<typename T_span, size_t A_leaf = 4096, size_t A_branch = 4096>
struct t_spans
{
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
		bool operator!=(const t_index& a_x) const
		{
			return !(*this == a_x);
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
		static constexpr auto f_get(T* a_base, T* a_p) -> decltype(a_p->f_get(0))
		{
			return a_p->f_get(f_delta(a_base, a_p).v_i1);
		}
		template<typename T>
		static constexpr t_index f_delta(T* a_base, T* a_p)
		{
			return {1, a_p > a_base ? a_p->v_n - a_p[-1].v_n : a_p->v_n};
		}
	};
	typedef jumoku::t_array<T_span, A_leaf, A_branch, t_traits> t_array;

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

}

#endif
