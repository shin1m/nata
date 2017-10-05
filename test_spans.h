#include "spans.h"

#include <cassert>

template<typename T, typename U>
void f_assert_equals(const T& a0, const U& a1)
{
	assert(std::equal(a0.f_begin(), a0.f_end(), a1.begin(), a1.end()));
}

template<typename T>
void f_assert_equals(const T& a0, std::initializer_list<typename T::t_span> a1)
{
	f_assert_equals<T, std::initializer_list<typename T::t_span>>(a0, a1);
}

template<typename T_value, size_t A_leaf, size_t A_branch>
void f_dump(const nata::t_spans<T_value, A_leaf, A_branch>& a)
{
	for (auto i = a.f_begin(); i != a.f_end(); ++i) {
		auto x = *i;
		std::printf("%d, %d\n", x.v_x, x.v_n);
	}
}
