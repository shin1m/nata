#ifndef NATA__MODEL_H
#define NATA__MODEL_H

#include "signal.h"
#include "spans.h"
#include <jumoku/utf32.h>

namespace nata
{

struct t_line
{
	size_t v_n;

	bool operator==(const t_line& a_x) const
	{
		return v_n == a_x.v_n;
	}
	t_line f_get(size_t a_n) const
	{
		return {a_n};
	}
};

template<size_t A_leaf, size_t A_branch>
struct t_lines : t_spans<t_line, A_leaf, A_branch>
{
	using t_span = t_line;
	using t_iterator = typename t_spans<t_line, A_leaf, A_branch>::t_iterator;

	t_lines()
	{
		this->v_array.f_insert(this->f_end(), t_span{1});
	}
	template<typename T>
	void f_replace(size_t a_p, size_t a_n, T a_first, T a_last)
	{
		auto i = this->f_at_in_text(a_p);
		size_t n = a_p - i.f_index().v_i1;
		size_t p = a_p + a_n;
		auto j = this->f_at_in_text(p);
		size_t m = (++j).f_index().v_i1 - p;
		i = this->v_array.f_erase(i, j);
		while (a_first != a_last) {
			++n;
			if (*a_first == L'\n') {
				i = this->v_array.f_insert(i, t_span{n});
				++i;
				n = 0;
			}
			++a_first;
		}
		n += m;
		if (n > 0) this->v_array.f_insert(i, t_span{n});
	}
};

template<typename T_lines = t_lines<256, 256>, size_t A_leaf = 256, size_t A_branch = 256>
class t_text : jumoku::t_utf8<A_leaf, A_branch>
{
	using t_array = jumoku::t_utf8<A_leaf, A_branch>;

	T_lines v_lines;

public:
	using t_iterator = jumoku::t_utf32_iterator<typename t_array::t_iterator>;

	t_signal<size_t, size_t, size_t> v_replaced;

	size_t f_size() const
	{
		return t_array::f_size().v_character;
	}
	t_iterator f_begin() const
	{
		return t_array::f_begin();
	}
	t_iterator f_end() const
	{
		return t_array::f_end();
	}
	t_iterator f_at(size_t a_index) const
	{
		return t_array::f_at(a_index, jumoku::t_utf8_traits::t_in_characters());
	}
	const T_lines& f_lines() const
	{
		return v_lines;
	}
	template<typename T>
	T f_slice(size_t a_p, size_t a_n, T a_out) const
	{
		for (auto i = f_at(a_p).f_base(); a_n > 0; --a_n) {
			*a_out++ = jumoku::t_utf32_traits::f_decode(*i, [&]
			{
				return *++i;
			});
			++i;
		}
		return a_out;
	}
	template<typename T>
	void f_replace(size_t a_p, size_t a_n, T a_first, T a_last)
	{
		auto i = this->f_erase(f_at(a_p).f_base(), f_at(a_p + a_n).f_base());
		auto n = f_size();
		{
			auto j = jumoku::f_utf8_inserter(*this, i);
			std::copy(a_first, a_last, jumoku::f_utf32_output(j));
		}
		v_lines.f_replace(a_p, a_n, a_first, a_last);
		v_replaced(a_p, a_n, f_size() - n);
	}
};

}

#endif
