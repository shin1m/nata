#ifndef NATA__TOKENS_H
#define NATA__TOKENS_H

#include "signal.h"
#include "stretches.h"
#include <numeric>

namespace nata
{

template<typename T_text, typename T_value, size_t A_leaf = 256, size_t A_branch = 256>
class t_tokens : t_stretches<T_value, A_leaf, A_branch>
{
	using t_stretches = nata::t_stretches<T_value, A_leaf, A_branch>;

	t_slot<size_t, size_t, size_t> v_text_replaced = [this](auto a_p, auto a_n0, auto a_n1)
	{
		std::deque<typename t_stretches::t_span> xs;
		if (a_n1 > 0) xs.push_back({{}, a_n1});
		this->f_replace(a_p, a_n0, std::move(xs));
		v_replaced(a_p, a_n0, a_n1);
	};

public:
	using t_span = typename t_stretches::t_span;
	using t_iterator = typename t_stretches::t_iterator;

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

}

#endif
