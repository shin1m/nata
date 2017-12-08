#ifndef NATA__STRETCHES_H
#define NATA__STRETCHES_H

#include "spans.h"
#include <deque>

namespace nata
{

template<typename T_value, size_t A_leaf = 4096, size_t A_branch = 4096>
struct t_stretches : t_spans<t_span<T_value>, A_leaf, A_branch>
{
	typedef nata::t_span<T_value> t_span;
	typedef typename t_spans<t_span, A_leaf, A_branch>::t_iterator t_iterator;

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
		a_xs.clear();
		if (i.f_index().v_i1 + i.f_delta().v_i1 == a_p) ++i;
		return i;
	}
};

}

#endif
