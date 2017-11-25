#include "foldings.h"
#include "test_spans.h"
#include <vector>

struct t_test_folding_span;

struct t_test_folding
{
	std::vector<t_test_folding_span> v_foldings;
	bool v_folded;
	bool v_null;

	t_test_folding() : v_null(true)
	{
	}
	t_test_folding(std::vector<t_test_folding_span>&& a_foldings, bool a_folded) : v_foldings(std::move(a_foldings)), v_folded(a_folded), v_null(false)
	{
	}
};

struct t_test_folding_span
{
	t_test_folding v_x;
	size_t v_n;
};

template<typename T>
inline bool operator==(const T& a_x, const t_test_folding_span& a_y)
{
	if (!a_x.v_x != a_y.v_x.v_null) return false;
	if (a_x.v_x) {
		auto& x = a_x.v_x->v_foldings;
		auto& y = a_y.v_x.v_foldings;
		if (!std::equal(x.f_begin(), x.f_end(), y.begin(), y.end())) return false;
		if (a_x.v_x->v_folded != a_y.v_x.v_folded) return false;
	}
	return a_x.v_n == a_y.v_n;
}

template<typename T>
void f_assert_equals(const T& a0, std::initializer_list<t_test_folding_span> a1)
{
	f_assert_equals<T, std::initializer_list<t_test_folding_span>>(a0, a1);
}

template<size_t A_leaf, size_t A_branch>
void f_dump(const nata::t_foldings<A_leaf, A_branch>& a, const std::string& a_indent)
{
	for (auto i = a.f_begin(); i != a.f_end(); ++i) {
		auto x = *i;
		if (x.v_x) {
			std::printf("%s{\n", a_indent.c_str());
			f_dump(x.v_x->v_foldings, a_indent + '\t');
			std::printf("%s}, %d, %d\n", a_indent.c_str(), x.v_x->v_folded, x.v_n);
		} else {
			std::printf("%s%d\n", a_indent.c_str(), x.v_n);
		}
	}
}

int main(int argc, char* argv[])
{
	{
		nata::t_foldings<5, 5> foldings;
		auto i = foldings.f_replace(0, 0, {
			{10}
		});
		f_assert_equals(foldings, {
			{{}, 10}
		});
		assert(i == foldings.f_begin());
		// ----------
		//  \
		// ----------
		i = foldings.f_replace(1, 0, {});
		f_assert_equals(foldings, {
			{{}, 10}
		});
		assert(i == foldings.f_begin());
	}
	{
		nata::t_foldings<5, 5> foldings;
		foldings.f_replace(0, 0, {
			{10}
		});
		// ----------
		//  | \
		// -===-------
		foldings.f_replace(1, 2, {
			{3}
		});
		f_assert_equals(foldings, {
			{{}, 11}
		});
	}
	{
		nata::t_foldings<5, 5> foldings;
		foldings.f_replace(0, 0, {
			{10}
		});
		// ----------
		//  | \
		// -===-------
		//  ===
		foldings.f_replace(1, 2, {
			{{{3}}}
		});
		f_assert_equals(foldings, {
			{{}, 1},
			{{{
				{{}, 3}
			}, false}, 3},
			{{}, 7}
		});
	}
	{
		nata::t_foldings<5, 5> foldings;
		foldings.f_replace(0, 0, {
			{1},
			{{{2}}},
			{7}
		});
		f_assert_equals(foldings, {
			{{}, 1},
			{{{
				{{}, 2}
			}, false}, 2},
			{{}, 7}
		});
		// ----------
		//  --
		//   | \
		// --===------
		foldings.f_replace(2, 2, {
			{3}
		});
		f_assert_equals(foldings, {
			{{}, 11}
		});
	}
	{
		nata::t_foldings<5, 5> foldings;
		foldings.f_replace(0, 0, {
			{1},
			{{
				{1},
				{{{2}}}
			}},
			{6}
		});
		f_assert_equals(foldings, {
			{{}, 1},
			{{{
				{{}, 1},
				{{{
					{{}, 2}
				}, false}, 2}
			}, false}, 3},
			{{}, 6}
		});
		// ----------
		//  ---
		//   --
		//   |  \
		// --====-----
		foldings.f_replace(2, 3, {
			{4}
		});
		f_assert_equals(foldings, {
			{{}, 11}
		});
	}
	{
		nata::t_foldings<5, 5> foldings;
		foldings.f_replace(0, 0, {
			{1},
			{{{2}}},
			{7}
		});
		// ----------
		//  --
		//  |\
		// -==--------
		foldings.f_replace(1, 1, {
			{2}
		});
		f_assert_equals(foldings, {
			{{}, 11}
		});
	}
	{
		nata::t_foldings<5, 5> foldings;
		foldings.f_replace(0, 0, {
			{1},
			{{
				{{{2}}},
				{1}
			}},
			{6}
		});
		f_assert_equals(foldings, {
			{{}, 1},
			{{{
				{{{
					{{}, 2}
				}, false}, 2},
				{{}, 1}
			}, false}, 3},
			{{}, 6}
		});
		// ----------
		//  ---
		//  --
		//  | \
		// -===-------
		foldings.f_replace(1, 2, {
			{3}
		});
		f_assert_equals(foldings, {
			{{}, 11}
		});
	}
	{
		nata::t_foldings<5, 5> foldings;
		foldings.f_replace(0, 0, {
			{1},
			{{{2}}},
			{7}
		});
		// ----------
		//  --
		//  | \
		// -|-------
		foldings.f_replace(1, 2, {});
		f_assert_equals(foldings, {
			{{}, 8}
		});
	}
	{
		nata::t_foldings<5, 5> foldings;
		foldings.f_replace(0, 0, {
			{1},
			{{{2}}},
			{{{2}}},
			{5}
		});
		f_assert_equals(foldings, {
			{{}, 1},
			{{{
				{{}, 2}
			}, false}, 2},
			{{{
				{{}, 2}
			}, false}, 2},
			{{}, 5}
		});
		// --- -------
		//  -- --
		//  | \
		// -|-------
		//   --
		foldings.f_replace(1, 2, {});
		f_assert_equals(foldings, {
			{{}, 1},
			{{{
				{{}, 2}
			}, false}, 2},
			{{}, 5}
		});
	}
	{
		nata::t_foldings<5, 5> foldings;
		foldings.f_replace(0, 0, {
			{1},
			{{{2}}},
			{{{2}}},
			{5}
		});
		// --- -------
		//  -- --
		//     | \
		// --- |-----
		//  --
		foldings.f_replace(3, 2, {});
		f_assert_equals(foldings, {
			{{}, 1},
			{{{
				{{}, 2}
			}, false}, 2},
			{{}, 5}
		});
	}
	{
		nata::t_foldings<5, 5> foldings;
		foldings.f_replace(0, 0, {
			{1},
			{{{4}}},
			{5}
		});
		f_assert_equals(foldings, {
			{{}, 1},
			{{{
				{{}, 4}
			}, false}, 4},
			{{}, 5}
		});
		// ----------
		//  ----
		//   |  \
		// --====-----
		//  -====
		foldings.f_replace(2, 3, {
			{4}
		});
		f_assert_equals(foldings, {
			{{}, 1},
			{{{
				{{}, 5}
			}, false}, 5},
			{{}, 5}
		});
	}
	{
		nata::t_foldings<5, 5> foldings;
		auto i = foldings.f_replace(0, 0, {
			{10}
		});
		f_assert_equals(foldings, {
			{{}, 10}
		});
		assert(i == foldings.f_begin());
		i = foldings.f_replace(1, 2, {
			{{{3}}}
		});
		f_assert_equals(foldings, {
			{{}, 1},
			{{{
				{{}, 3}
			}, false}, 3},
			{{}, 7}
		});
		assert(i == foldings.f_begin());
	}
	{
		nata::t_foldings<5, 5> foldings;
		foldings.f_replace(0, 0, {
			{2},
			{{{5}}},
			{3}
		});
		f_assert_equals(foldings, {
			{{}, 2},
			{{{
				{{}, 5}
			}, false}, 5},
			{{}, 3}
		});
		f_assert_equals(foldings, {
			{{}, 2},
			{{{
				{{}, 5}
			}, false}, 5},
			{{}, 3}
		});
		foldings.f_replace(7, 1, {});
		f_assert_equals(foldings, {
			{{}, 2},
			{{{
				{{}, 5}
			}, false}, 5},
			{{}, 2}
		});
	}
	{
		nata::t_foldings<5, 5> foldings;
		foldings.f_replace(0, 0, {
			{2},
			{{{5}}},
			{3}
		});
		foldings.f_replace(2, 1, {});
		f_assert_equals(foldings, {
			{{}, 9}
		});
	}
	{
		nata::t_foldings<5, 5> foldings;
		foldings.f_replace(0, 0, {
			{1},
			{{{7}}},
			{2}
		});
		f_assert_equals(foldings, {
			{{}, 1},
			{{{
				{{}, 7}
			}, false}, 7},
			{{}, 2}
		});
		foldings.f_replace(3, 4, {
			{{{4}}}
		});
		f_assert_equals(foldings, {
			{{}, 1},
			{{{
				{{}, 2},
				{{{
					{{}, 4}
				}, false}, 4},
				{{}, 1}
			}, false}, 7},
			{{}, 2}
		});
	}
	{
		nata::t_foldings<5, 5> foldings;
		foldings.f_replace(0, 0, {
			{1},
			{{
				{2},
				{{{4}}},
				{1}
			}},
			{2}
		});
		f_assert_equals(foldings, {
			{{}, 1},
			{{{
				{{}, 2},
				{{{
					{{}, 4}
				}, false}, 4},
				{{}, 1}
			}, false}, 7},
			{{}, 2}
		});
		foldings.f_replace(2, 1, {
			{3}
		});
		f_assert_equals(foldings, {
			{{}, 1},
			{{{
				{{}, 4},
				{{{
					{{}, 4}
				}, false}, 4},
				{{}, 1}
			}, false}, 9},
			{{}, 2}
		});
	}
	return 0;
}
