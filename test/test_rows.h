#include "view.h"
#include "test_spans.h"

template<typename T>
void f_assert_equals(const T& a0, std::initializer_list<typename T::t_row> a1)
{
	f_assert_equals<T, std::initializer_list<typename T::t_row>>(a0, a1);
}

template<typename T_tokens, typename T_foldings, typename T_target, size_t A_leaf, size_t A_branch>
void f_dump(const nata::t_rows<T_tokens, T_foldings, T_target, A_leaf, A_branch>& a)
{
	for (auto i = a.f_begin(); i != a.f_end(); ++i) {
		auto x = *i;
		std::printf("%d, %d, %d, %d, %d, %d\n", x.v_line, x.v_tail, x.v_text, x.v_width, x.v_ascent, x.v_height);
	}
}

struct t_test_target
{
	size_t v_width = 10;
	nata::t_signal<> v_resized;

	size_t f_width() const
	{
		return v_width;
	}
	std::tuple<size_t, size_t, size_t> f_size(wchar_t a_c, int a_a) const
	{
		return {1 + a_a, 2, a_a};
	}
	std::tuple<size_t, size_t, size_t> f_tab(size_t a_x, int a_a) const
	{
		return {(a_x + 4) / 4 * 4 - a_x, 2, 1};
	}
	std::tuple<size_t, size_t, size_t> f_eol(int a_a) const
	{
		return {1, 2, a_a};
	}
	std::tuple<size_t, size_t, size_t> f_eof() const
	{
		return {1, 1, 0};
	}
	std::tuple<size_t, size_t, size_t> f_folded() const
	{
		return {3, 1, 0};
	}
};
