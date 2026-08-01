#ifndef NATA__MARKS_H
#define NATA__MARKS_H

#include "signal.h"
#include "spans.h"

namespace nata
{

template<typename T_value>
struct t_mark : t_span<T_value>
{
	double v_d;

	bool operator==(const t_mark& a_x) const
	{
		return static_cast<const t_span<T_value>&>(*this) == a_x && v_d == a_x.v_d;
	}
	t_mark f_get(size_t a_n, double a_d) const
	{
		return {this->v_x, a_n, a_d};
	}
};

template<typename T_text, typename T_value, size_t A_leaf = 8, size_t A_branch = 8>
class t_marks
{
	template<typename T>
	struct t_index
	{
		T v_i0;
		T v_i1;
		double v_i2;

		template<typename U>
		operator t_index<U>() const
		{
			return {U(v_i0), U(v_i1), v_i2};
		}
		t_index<int> operator-() const
		{
			return {-int(v_i0), -int(v_i1), -v_i2};
		}
		bool operator==(const t_index& a_x) const
		{
			return v_i0 == a_x.v_i0 && v_i1 == a_x.v_i1 && v_i2 == a_x.v_i2;
		}
		template<typename U>
		t_index& operator+=(const t_index<U>& a_x)
		{
			v_i0 += a_x.v_i0;
			v_i1 += a_x.v_i1;
			v_i2 += a_x.v_i2;
			return *this;
		}
		template<typename U>
		t_index& operator-=(const t_index<U>& a_x)
		{
			v_i0 -= a_x.v_i0;
			v_i1 -= a_x.v_i1;
			v_i2 -= a_x.v_i2;
			return *this;
		}
		template<typename U>
		t_index operator+(const t_index<U>& a_x) const
		{
			return {v_i0 + T(a_x.v_i0), v_i1 + T(a_x.v_i1), v_i2 + a_x.v_i2};
		}
		template<typename U>
		t_index operator-(const t_index<U>& a_x) const
		{
			return {v_i0 - T(a_x.v_i0), v_i1 - T(a_x.v_i1), v_i2 - a_x.v_i2};
		}
	};
	struct t_traits
	{
		using t_index = t_marks::t_index<size_t>;
		using t_delta = t_marks::t_index<int>;
		struct t_default
		{
			constexpr size_t operator()(const t_index& a_index) const
			{
				return a_index.v_i0;
			}
		};

		static constexpr t_index f_index(size_t a_n, const auto& a_value)
		{
			return {a_n, a_value.v_n, a_value.v_d};
		}
		static constexpr void f_add(auto& a_value, const t_delta& a_index)
		{
			a_value.v_n += a_index.v_i1;
			a_value.v_d += a_index.v_i2;
		}
		static constexpr auto f_get(auto* a_base, auto* a_p) -> decltype(a_p->f_get(0, 0.0))
		{
			auto d = f_delta(a_base, a_p);
			return a_p->f_get(d.v_i1, d.v_i2);
		}
		static constexpr t_index f_delta(auto* a_base, auto* a_p)
		{
			if (a_p > a_base) {
				auto& x = a_p[-1];
				return {1, a_p->v_n - x.v_n, a_p->v_d - x.v_d};
			}
			return {1, a_p->v_n, a_p->v_d};
		}
	};
	using t_array = jumoku::t_array<t_mark<T_value>, A_leaf, A_branch, t_traits>;

	t_array v_array;

	t_array::t_constant_iterator f_at_text(size_t a_p) const
	{
		return v_array.f_at(a_p, [](const auto& a_index)
		{
			return a_index.v_i1;
		});
	}
	typename T_text::t_replaced v_text_replaced = [this](auto a_p, auto a_n0, auto a_n1)
	{
		auto p = a_p.v_character;
		auto n0 = a_n0.v_character;
		auto j = f_first_at_text(p + n0);
		auto i = n0 > 0 ? f_last_at_text(p) : j;
		if (j.f_index().v_i1 < p + n0 + 1)
			++j;
		else if (n0 <= 0)
			--i;
		auto delta = j.f_index() - i.f_index();
		v_array.f_insert(v_array.f_erase(i, j), t_span{{{}, delta.v_i1 - n0 + a_n1.v_character}, delta.v_i2});
	};

public:
	using t_span = t_mark<T_value>;
	using t_iterator = typename t_array::t_constant_iterator;

	T_text& v_text;

	t_marks(T_text& a_text) : v_text(a_text)
	{
		v_text.v_replaced >> v_text_replaced;
		v_array.f_insert(f_end(), t_span{{{}, v_text.f_size() + 2}, 1.0});
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
	t_iterator f_first_at_text(size_t a_p) const
	{
		auto i = f_at_text(a_p);
		auto j = i;
		return (++j).f_index().v_i1 == a_p + 1 ? j : i;
	}
	t_iterator f_last_at_text(size_t a_p) const
	{
		return f_at_text(a_p + 1);
	}
	t_iterator f_at_key(double a_p) const
	{
		return v_array.f_at(a_p, [](const auto& a_index)
		{
			return a_index.v_i2;
		});
	}
	t_iterator f_insert(size_t a_p, T_value&& a_value)
	{
		auto i = f_last_at_text(a_p);
		++a_p;
		auto p = i.f_index().v_i1;
		if (p == a_p) {
			auto delta = (--i).f_delta();
			auto d = delta.v_i2 * 0.5;
			t_span xs[] = {{i->v_x, delta.v_i1, d}, {std::move(a_value), 0, d}};
			return ++v_array.f_insert(v_array.f_erase(i), std::begin(xs), std::end(xs));
		}
		auto n = a_p - p;
		auto delta = i.f_delta();
		auto d = delta.v_i2 * 0.5;
		t_span xs[] = {{{{}, n}, d * 0.5}, {std::move(a_value), 0, d * 0.5}, {{{}, delta.v_i1 - n}, d}};
		return ++v_array.f_insert(v_array.f_erase(i), std::begin(xs), std::end(xs));
	}
	t_iterator f_erase(t_iterator a_i)
	{
		if (a_i.f_delta().v_i1) return a_i;
		auto i = a_i;
		--a_i;
		++i;
		if (a_i.f_delta().v_i1 && i.f_delta().v_i1) ++i;
		auto delta = i.f_index() - a_i.f_index();
		return ++v_array.f_insert(v_array.f_erase(a_i, i), t_span{a_i->v_x, delta.v_i1, delta.v_i2});
	}
};

}

#endif
