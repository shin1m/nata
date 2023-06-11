#include <nata/nested.h>
#include "test_spans.h"
#include <numeric>
#include <string>

template<size_t A_leaf, size_t A_branch>
struct t_test_value
{
	nata::t_nested<t_test_value, A_leaf, A_branch> v_nested;
};

struct t_test_span
{
	std::shared_ptr<std::vector<t_test_span>> v_x;
	size_t v_n;

	t_test_span(size_t a_n) : v_n(a_n)
	{
	}
	t_test_span(std::vector<t_test_span>&& a_nested) : v_x(std::make_shared<std::vector<t_test_span>>(std::move(a_nested)))
	{
		v_n = std::accumulate(v_x->begin(), v_x->end(), 0, [](size_t n, const auto& x)
		{
			return n + x.v_n;
		});
	}
};

inline bool operator==(const auto& a_x, const t_test_span& a_y)
{
	if (!a_x.v_x != !a_y.v_x) return false;
	if (a_x.v_x) {
		auto& x = a_x.v_x->v_nested;
		auto& y = *a_y.v_x;
		if (!std::equal(x.f_begin(), x.f_end(), y.begin(), y.end())) return false;
	}
	return a_x.v_n == a_y.v_n;
}

template<typename T>
void f_assert_nested(const T& a0, std::initializer_list<t_test_span> a1)
{
	f_assert_equals<T, std::initializer_list<t_test_span>>(a0, a1);
}

template<template<size_t, size_t> typename T_value, size_t A_leaf, size_t A_branch>
void f_dump(const nata::t_nested<T_value, A_leaf, A_branch>& a, const std::string& a_indent)
{
	for (auto i = a.f_begin(); i != a.f_end(); ++i) {
		auto x = *i;
		if (x.v_x) {
			std::printf("%s{\n", a_indent.c_str());
			f_dump(x.v_x->v_nested, a_indent + '\t');
			std::printf("%s}, %d\n", a_indent.c_str(), x.v_n);
		} else {
			std::printf("%s%d\n", a_indent.c_str(), x.v_n);
		}
	}
}
