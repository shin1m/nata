#include "foldings.h"
#include "test_spans.h"
#include <numeric>
#include <vector>

struct t_test_traits
{
	template<size_t A_leaf, size_t A_branch>
	struct t_folding
	{
		nata::t_foldings<t_test_traits, A_leaf, A_branch> v_foldings;
	};
	template<size_t A_leaf, size_t A_branch>
	struct t_span
	{
		std::shared_ptr<t_folding<A_leaf, A_branch>> v_x;
		size_t v_n;

		t_span() = default;
		t_span(const std::shared_ptr<t_folding<A_leaf, A_branch>>& a_x, size_t a_n) : v_x(std::move(a_x)), v_n(a_n)
		{
		}
		t_span(std::shared_ptr<t_folding<A_leaf, A_branch>>&& a_x) : v_x(std::move(a_x)), v_n(v_x->v_foldings.f_size().v_i1)
		{
		}
		t_span(size_t a_n) : v_n(a_n)
		{
		}
		t_span(std::deque<t_span>&& a_xs)
		{
			v_x = std::make_shared<t_folding<A_leaf, A_branch>>();
			v_x->v_foldings.f_replace(0, 0, std::move(a_xs));
			v_n = v_x->v_foldings.f_size().v_i1;
		}
		bool operator==(const t_span& a_x) const
		{
			return v_x == a_x.v_x && v_n == a_x.v_n;
		}
		t_span f_get(size_t a_n) const
		{
			return {v_x, a_n};
		}
	};
};

struct t_test_span
{
	std::shared_ptr<std::vector<t_test_span>> v_x;
	size_t v_n;

	t_test_span(size_t a_n) : v_n(a_n)
	{
	}
	t_test_span(std::vector<t_test_span>&& a_foldings) : v_x(std::make_shared<std::vector<t_test_span>>(std::move(a_foldings)))
	{
		v_n = std::accumulate(v_x->begin(), v_x->end(), 0, [](size_t n, const auto& x)
		{
			return n + x.v_n;
		});
	}
};

template<typename T>
inline bool operator==(const T& a_x, const t_test_span& a_y)
{
	if (!a_x.v_x != !a_y.v_x) return false;
	if (a_x.v_x) {
		auto& x = a_x.v_x->v_foldings;
		auto& y = *a_y.v_x;
		if (!std::equal(x.f_begin(), x.f_end(), y.begin(), y.end())) return false;
	}
	return a_x.v_n == a_y.v_n;
}

template<typename T>
void f_assert_equals(const T& a0, std::initializer_list<t_test_span> a1)
{
	f_assert_equals<T, std::initializer_list<t_test_span>>(a0, a1);
}

template<typename T_traits, size_t A_leaf, size_t A_branch>
void f_dump(const nata::t_foldings<T_traits, A_leaf, A_branch>& a, const std::string& a_indent)
{
	for (auto i = a.f_begin(); i != a.f_end(); ++i) {
		auto x = *i;
		if (x.v_x) {
			std::printf("%s{\n", a_indent.c_str());
			f_dump(x.v_x->v_foldings, a_indent + '\t');
			std::printf("%s}, %d\n", a_indent.c_str(), x.v_n);
		} else {
			std::printf("%s%d\n", a_indent.c_str(), x.v_n);
		}
	}
}

int main(int argc, char* argv[])
{
	{
		nata::t_foldings<t_test_traits, 5, 5> foldings;
		auto i = foldings.f_replace(0, 0, {
			{10}
		});
		f_assert_equals(foldings, {
			{10}
		});
		assert(i == foldings.f_begin());
		// ----------
		//  \
		// ----------
		i = foldings.f_replace(1, 0, {});
		f_assert_equals(foldings, {
			{10}
		});
		assert(i == foldings.f_begin());
	}
	{
		nata::t_foldings<t_test_traits, 5, 5> foldings;
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
			{11}
		});
	}
	{
		nata::t_foldings<t_test_traits, 5, 5> foldings;
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
			{1},
			{{{3}}},
			{7}
		});
	}
	{
		nata::t_foldings<t_test_traits, 5, 5> foldings;
		foldings.f_replace(0, 0, {
			{1},
			{{{2}}},
			{7}
		});
		f_assert_equals(foldings, {
			{1},
			{{{2}}},
			{7}
		});
		// ----------
		//  --
		//   | \
		// --===------
		foldings.f_replace(2, 2, {
			{3}
		});
		f_assert_equals(foldings, {
			{11}
		});
	}
	{
		nata::t_foldings<t_test_traits, 5, 5> foldings;
		foldings.f_replace(0, 0, {
			{1},
			{{
				{1},
				{{{2}}}
			}},
			{6}
		});
		f_assert_equals(foldings, {
			{1},
			{{
				{1},
				{{{2}}}
			}},
			{6}
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
			{11}
		});
	}
	{
		nata::t_foldings<t_test_traits, 5, 5> foldings;
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
			{11}
		});
	}
	{
		nata::t_foldings<t_test_traits, 5, 5> foldings;
		foldings.f_replace(0, 0, {
			{1},
			{{
				{{{2}}},
				{1}
			}},
			{6}
		});
		f_assert_equals(foldings, {
			{1},
			{{
				{{{2}}},
				{1}
			}},
			{6}
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
			{11}
		});
	}
	{
		nata::t_foldings<t_test_traits, 5, 5> foldings;
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
			{8}
		});
	}
	{
		nata::t_foldings<t_test_traits, 5, 5> foldings;
		foldings.f_replace(0, 0, {
			{1},
			{{{2}}},
			{{{2}}},
			{5}
		});
		f_assert_equals(foldings, {
			{1},
			{{{2}}},
			{{{2}}},
			{5}
		});
		// --- -------
		//  -- --
		//  | \
		// -|-------
		//   --
		foldings.f_replace(1, 2, {});
		f_assert_equals(foldings, {
			{1},
			{{{2}}},
			{5}
		});
	}
	{
		nata::t_foldings<t_test_traits, 5, 5> foldings;
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
			{1},
			{{{2}}},
			{5}
		});
	}
	{
		nata::t_foldings<t_test_traits, 5, 5> foldings;
		foldings.f_replace(0, 0, {
			{1},
			{{{4}}},
			{5}
		});
		f_assert_equals(foldings, {
			{1},
			{{{4}}},
			{5}
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
			{1},
			{{{5}}},
			{5}
		});
	}
	{
		nata::t_foldings<t_test_traits, 5, 5> foldings;
		auto i = foldings.f_replace(0, 0, {
			{10}
		});
		f_assert_equals(foldings, {
			{10}
		});
		assert(i == foldings.f_begin());
		i = foldings.f_replace(1, 2, {
			{{{3}}}
		});
		f_assert_equals(foldings, {
			{1},
			{{{3}}},
			{7}
		});
		assert(i == foldings.f_begin());
	}
	{
		nata::t_foldings<t_test_traits, 5, 5> foldings;
		foldings.f_replace(0, 0, {
			{2},
			{{{5}}},
			{3}
		});
		f_assert_equals(foldings, {
			{2},
			{{{5}}},
			{3}
		});
		foldings.f_replace(7, 1, {});
		f_assert_equals(foldings, {
			{2},
			{{{5}}},
			{2}
		});
	}
	{
		nata::t_foldings<t_test_traits, 5, 5> foldings;
		foldings.f_replace(0, 0, {
			{2},
			{{{5}}},
			{3}
		});
		foldings.f_replace(2, 1, {});
		f_assert_equals(foldings, {
			{9}
		});
	}
	{
		nata::t_foldings<t_test_traits, 5, 5> foldings;
		foldings.f_replace(0, 0, {
			{1},
			{{{7}}},
			{2}
		});
		f_assert_equals(foldings, {
			{1},
			{{{7}}},
			{2}
		});
		foldings.f_replace(3, 4, {
			{{{4}}}
		});
		f_assert_equals(foldings, {
			{1},
			{{
				{2},
				{{{4}}},
				{1}
			}},
			{2}
		});
	}
	{
		nata::t_foldings<t_test_traits, 5, 5> foldings;
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
			{1},
			{{
				{2},
				{{{4}}},
				{1}
			}},
			{2}
		});
		foldings.f_replace(2, 1, {
			{3}
		});
		f_assert_equals(foldings, {
			{1},
			{{
				{4},
				{{{4}}},
				{1}
			}},
			{2}
		});
	}
	return 0;
}
