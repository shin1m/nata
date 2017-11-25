#ifndef NATA__FOLDINGS_H
#define NATA__FOLDINGS_H

#include "spans.h"

namespace nata
{

template<size_t A_leaf, size_t A_branch>
struct t_foldings;

template<size_t A_leaf, size_t A_branch>
struct t_folding
{
	t_foldings<A_leaf, A_branch> v_foldings;
	bool v_folded = false;
};

template<size_t A_leaf = 4096, size_t A_branch = 4096>
struct t_foldings : t_spans<std::shared_ptr<t_folding<A_leaf, A_branch>>, A_leaf, A_branch>
{
	typedef typename std::shared_ptr<t_folding<A_leaf, A_branch>> t_value;
	typedef typename t_spans<t_value, A_leaf, A_branch>::t_span t_base_span;
	typedef typename t_spans<t_value, A_leaf, A_branch>::t_iterator t_iterator;
	struct t_span : t_base_span
	{
		t_span(size_t a_n)
		{
			this->v_n = a_n;
		}
		t_span(std::deque<t_span>&& a_xs)
		{
			this->v_x = std::make_shared<t_folding<A_leaf, A_branch>>();
			this->v_x->v_foldings.f_replace(0, 0, std::move(a_xs));
			this->v_n = this->v_x->v_foldings.f_size().v_i1;
		}
		t_span(t_base_span&& a_x)
		{
			this->v_x = std::move(a_x.v_x);
			this->v_n = a_x.v_n;
		}
	};

private:
	template<typename T_span>
	void f_merge(t_iterator& a_i, std::deque<T_span>& a_xs) const
	{
		if (a_xs.front().v_x || a_i == this->f_begin()) return;
		auto i = a_i;
		if ((--i)->v_x) return;
		a_xs.front().v_n += i.f_delta().v_i1;
		a_i = i;
	}
	template<typename T_span>
	void f_merge(t_iterator a_i, size_t a_n, std::deque<T_span>& a_xs) const
	{
		if (a_i->v_x) {
			auto& x = a_i->v_x->v_foldings;
			auto i = x.f_at_in_text(a_n);
			size_t n = a_n - i.f_index().v_i1;
			if (n > 0) x.f_merge(i, n, a_xs);
			if (!a_xs.empty()) x.f_merge(i, a_xs);
			a_xs.insert(a_xs.begin(), x.f_begin(), i);
		} else if (a_xs.empty() || a_xs.front().v_x) {
			a_xs.emplace_front(a_n);
		} else {
			a_xs.front().v_n += a_n;
		}
	}
	template<typename T_span>
	void f_merge(std::deque<T_span>& a_xs, t_iterator& a_i) const
	{
		if (a_xs.back().v_x || a_i == this->f_end() || a_i->v_x) return;
		a_xs.back().v_n += a_i.f_delta().v_i1;
		++a_i;
	}
	template<typename T_span>
	void f_merge(std::deque<T_span>& a_xs, t_iterator a_i, size_t a_n) const
	{
		if (a_i->v_x) {
			auto& x = a_i->v_x->v_foldings;
			auto i = x.f_at_in_text(a_n);
			size_t n = a_n - i.f_index().v_i1;
			if (n > 0) {
				x.f_merge(a_xs, i, n);
				++i;
			}
			if (!a_xs.empty()) x.f_merge(a_xs, i);
			a_xs.insert(a_xs.end(), i, x.f_end());
		} else {
			size_t n = a_i.f_delta().v_i1 - a_n;
			if (a_xs.empty() || a_xs.back().v_x)
				a_xs.emplace_back(n);
			else
				a_xs.back().v_n += n;
		}
	}

public:
	template<typename T_span>
	t_iterator f_replace(size_t a_p, size_t a_n, std::deque<T_span>&& a_xs)
	{
		auto i = this->f_at_in_text(a_p);
		if (a_n <= 0 && a_xs.empty()) return i;
		size_t n = a_p - i.f_index().v_i1;
		size_t p = a_p + a_n;
		if (n > 0) {
			if (i->v_x && p <= i.f_index().v_i1 + i.f_delta().v_i1) {
				auto x = i->v_x;
				x->v_foldings.f_replace(n, a_n, std::move(a_xs));
				return this->v_array.f_insert(this->v_array.f_erase(i), t_base_span{x, x->v_foldings.f_size().v_i1});
			}
			f_merge(i, n, a_xs);
		}
		auto j = this->f_at_in_text(p);
		n = p - j.f_index().v_i1;
		if (n > 0) {
			f_merge(a_xs, j, n);
			++j;
		}
		if (!a_xs.empty()) {
			f_merge(i, a_xs);
			f_merge(a_xs, j);
		} else if (i != this->f_begin() && j != this->f_end() && !j->v_x) {
			auto k = i;
			if (!(--k)->v_x) {
				a_xs.emplace_back(k.f_delta().v_i1 + j.f_delta().v_i1);
				i = k;
				++j;
			}
		}
		return this->v_array.f_insert(this->v_array.f_erase(i, j), a_xs.begin(), a_xs.end());
	}
	t_iterator f_replace(size_t a_p, size_t a_n, std::deque<t_span>&& a_xs)
	{
		return f_replace<t_span>(a_p, a_n, std::move(a_xs));
	}
};

}

#endif
