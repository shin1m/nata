#ifndef NATA__NESTED_H
#define NATA__NESTED_H

#include "spans.h"
#include <deque>
#include <vector>

namespace nata
{

template<template<size_t, size_t> typename T_value, size_t A_leaf, size_t A_branch>
class t_nested_span
{
	template<template<size_t, size_t> typename, size_t, size_t> friend struct t_nested;
	using t_value = T_value<A_leaf, A_branch>;

	t_nested_span(const std::shared_ptr<t_value>& a_x, size_t a_n) : v_x(a_x), v_n(a_n)
	{
	}
	t_nested_span(std::shared_ptr<t_value>&& a_x) : v_x(std::move(a_x)), v_n(v_x->v_nested.f_size().v_i1)
	{
	}

public:
	std::shared_ptr<t_value> v_x;
	size_t v_n;

	t_nested_span(size_t a_n) : v_n(a_n)
	{
	}
	t_nested_span(std::deque<t_nested_span>&& a_xs) : v_x(std::make_shared<t_value>())
	{
		v_x->v_nested.f_replace(0, 0, std::move(a_xs));
		v_n = v_x->v_nested.f_size().v_i1;
	}
	bool operator==(const t_nested_span& a_x) const
	{
		return v_x == a_x.v_x && v_n == a_x.v_n;
	}
	t_nested_span f_get(size_t a_n) const
	{
		return {v_x, a_n};
	}
};

template<template<size_t, size_t> typename T_value, size_t A_leaf, size_t A_branch>
struct t_nested : t_spans<t_nested_span<T_value, A_leaf, A_branch>, A_leaf, A_branch>
{
	using t_value = T_value<A_leaf, A_branch>;
	using t_span = t_nested_span<T_value, A_leaf, A_branch>;
	using t_iterator = typename t_spans<t_span, A_leaf, A_branch>::t_iterator;

private:
	void f_merge(t_iterator& a_i, std::deque<t_span>& a_xs) const
	{
		if (a_xs.front().v_x || a_i == this->f_begin()) return;
		auto i = a_i;
		if ((--i)->v_x) return;
		a_xs.front().v_n += i.f_delta().v_i1;
		a_i = i;
	}
	void f_merge(t_iterator a_i, size_t a_n, std::deque<t_span>& a_xs) const
	{
		if (a_i->v_x) {
			auto& x = a_i->v_x->v_nested;
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
	void f_merge(std::deque<t_span>& a_xs, t_iterator& a_i) const
	{
		if (a_xs.back().v_x || a_i == this->f_end() || a_i->v_x) return;
		a_xs.back().v_n += a_i.f_delta().v_i1;
		++a_i;
	}
	void f_merge(std::deque<t_span>& a_xs, t_iterator a_i, size_t a_n) const
	{
		if (a_i->v_x) {
			auto& x = a_i->v_x->v_nested;
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
	template<typename T_enter>
	size_t f_path_at_in_text(size_t a_p, std::vector<t_iterator>& a_path, T_enter a_enter) const
	{
		auto x = this;
		while (true) {
			auto i = x->f_at_in_text(a_p);
			a_path.push_back(i);
			a_p -= i.f_index().v_i1;
			if (a_p <= 0 || !i->v_x || !a_enter(i->v_x)) break;
			x = &i->v_x->v_nested;
		}
		return a_p;
	}
	template<typename T_enter>
	void f_leaf(std::vector<t_iterator>& a_path, T_enter a_enter) const
	{
		if (a_path.back() == this->f_end()) return;
		while (true) {
			auto& i = a_path.back();
			if (!i->v_x || !a_enter(i->v_x)) break;
			a_path.push_back(i->v_x->v_nested.f_begin());
		}
	}
	template<typename T_enter>
	size_t f_leaf_at_in_text(size_t a_p, std::vector<t_iterator>& a_path, T_enter a_enter) const
	{
		a_p = f_path_at_in_text(a_p, a_path, a_enter);
		if (a_p <= 0) f_leaf(a_path, a_enter);
		return a_p;
	}
	void f_next(std::vector<t_iterator>& a_path) const
	{
		while (a_path.size() >= 2) {
			if (++a_path.back() != a_path[a_path.size() - 2]->v_x->v_nested.f_end()) return;
			a_path.pop_back();
		}
		++a_path.back();
	}
	t_iterator f_replace(size_t a_p, size_t a_n, std::deque<t_span>&& a_xs)
	{
		auto i = this->f_at_in_text(a_p);
		if (a_n <= 0 && a_xs.empty()) return i;
		size_t n = a_p - i.f_index().v_i1;
		size_t p = a_p + a_n;
		if (n > 0) {
			if (i->v_x && p <= i.f_index().v_i1 + i.f_delta().v_i1) {
				auto x = i->v_x;
				x->v_nested.f_replace(n, a_n, std::move(a_xs));
				return this->v_array.f_insert(this->v_array.f_erase(i), t_span{std::move(x)});
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
		i = this->v_array.f_insert(this->v_array.f_erase(i, j), a_xs.begin(), a_xs.end());
		a_xs.clear();
		return i;
	}
};

}

#endif
