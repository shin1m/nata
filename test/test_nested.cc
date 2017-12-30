#include "test_nested.h"

int main(int argc, char* argv[])
{
	{
		nata::t_nested<t_test_value, 5, 5> nested;
		auto i = nested.f_replace(0, 0, {
			{10}
		});
		f_assert_equals(nested, {
			{10}
		});
		assert(i == nested.f_begin());
		// ----------
		//  \
		// ----------
		i = nested.f_replace(1, 0, {});
		f_assert_equals(nested, {
			{10}
		});
		assert(i == nested.f_begin());
	}
	{
		nata::t_nested<t_test_value, 5, 5> nested;
		nested.f_replace(0, 0, {
			{10}
		});
		// ----------
		//  | \
		// -===-------
		nested.f_replace(1, 2, {
			{3}
		});
		f_assert_equals(nested, {
			{11}
		});
	}
	{
		nata::t_nested<t_test_value, 5, 5> nested;
		nested.f_replace(0, 0, {
			{10}
		});
		// ----------
		//  | \
		// -===-------
		//  ===
		nested.f_replace(1, 2, {
			{{{3}}}
		});
		f_assert_equals(nested, {
			{1},
			{{{3}}},
			{7}
		});
	}
	{
		nata::t_nested<t_test_value, 5, 5> nested;
		nested.f_replace(0, 0, {
			{1},
			{{{2}}},
			{7}
		});
		f_assert_equals(nested, {
			{1},
			{{{2}}},
			{7}
		});
		// ----------
		//  --
		//   | \
		// --===------
		nested.f_replace(2, 2, {
			{3}
		});
		f_assert_equals(nested, {
			{11}
		});
	}
	{
		nata::t_nested<t_test_value, 5, 5> nested;
		nested.f_replace(0, 0, {
			{1},
			{{
				{1},
				{{{2}}}
			}},
			{6}
		});
		f_assert_equals(nested, {
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
		nested.f_replace(2, 3, {
			{4}
		});
		f_assert_equals(nested, {
			{11}
		});
	}
	{
		nata::t_nested<t_test_value, 5, 5> nested;
		nested.f_replace(0, 0, {
			{1},
			{{{2}}},
			{7}
		});
		// ----------
		//  --
		//  |\
		// -==--------
		nested.f_replace(1, 1, {
			{2}
		});
		f_assert_equals(nested, {
			{11}
		});
	}
	{
		nata::t_nested<t_test_value, 5, 5> nested;
		nested.f_replace(0, 0, {
			{1},
			{{
				{{{2}}},
				{1}
			}},
			{6}
		});
		f_assert_equals(nested, {
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
		nested.f_replace(1, 2, {
			{3}
		});
		f_assert_equals(nested, {
			{11}
		});
	}
	{
		nata::t_nested<t_test_value, 5, 5> nested;
		nested.f_replace(0, 0, {
			{1},
			{{{2}}},
			{7}
		});
		// ----------
		//  --
		//  | \
		// -|-------
		nested.f_replace(1, 2, {});
		f_assert_equals(nested, {
			{8}
		});
	}
	{
		nata::t_nested<t_test_value, 5, 5> nested;
		nested.f_replace(0, 0, {
			{1},
			{{{2}}},
			{{{2}}},
			{5}
		});
		f_assert_equals(nested, {
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
		nested.f_replace(1, 2, {});
		f_assert_equals(nested, {
			{1},
			{{{2}}},
			{5}
		});
	}
	{
		nata::t_nested<t_test_value, 5, 5> nested;
		nested.f_replace(0, 0, {
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
		nested.f_replace(3, 2, {});
		f_assert_equals(nested, {
			{1},
			{{{2}}},
			{5}
		});
	}
	{
		nata::t_nested<t_test_value, 5, 5> nested;
		nested.f_replace(0, 0, {
			{1},
			{{{4}}},
			{5}
		});
		f_assert_equals(nested, {
			{1},
			{{{4}}},
			{5}
		});
		// ----------
		//  ----
		//   |  \
		// --====-----
		//  -====
		nested.f_replace(2, 3, {
			{4}
		});
		f_assert_equals(nested, {
			{1},
			{{{5}}},
			{5}
		});
	}
	{
		nata::t_nested<t_test_value, 5, 5> nested;
		auto i = nested.f_replace(0, 0, {
			{10}
		});
		f_assert_equals(nested, {
			{10}
		});
		assert(i == nested.f_begin());
		i = nested.f_replace(1, 2, {
			{{{3}}}
		});
		f_assert_equals(nested, {
			{1},
			{{{3}}},
			{7}
		});
		assert(i == nested.f_begin());
	}
	{
		nata::t_nested<t_test_value, 5, 5> nested;
		nested.f_replace(0, 0, {
			{2},
			{{{5}}},
			{3}
		});
		f_assert_equals(nested, {
			{2},
			{{{5}}},
			{3}
		});
		nested.f_replace(7, 1, {});
		f_assert_equals(nested, {
			{2},
			{{{5}}},
			{2}
		});
	}
	{
		nata::t_nested<t_test_value, 5, 5> nested;
		nested.f_replace(0, 0, {
			{2},
			{{{5}}},
			{3}
		});
		nested.f_replace(2, 1, {});
		f_assert_equals(nested, {
			{9}
		});
	}
	{
		nata::t_nested<t_test_value, 5, 5> nested;
		nested.f_replace(0, 0, {
			{1},
			{{{7}}},
			{2}
		});
		f_assert_equals(nested, {
			{1},
			{{{7}}},
			{2}
		});
		nested.f_replace(3, 4, {
			{{{4}}}
		});
		f_assert_equals(nested, {
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
		nata::t_nested<t_test_value, 5, 5> nested;
		nested.f_replace(0, 0, {
			{1},
			{{
				{2},
				{{{4}}},
				{1}
			}},
			{2}
		});
		f_assert_equals(nested, {
			{1},
			{{
				{2},
				{{{4}}},
				{1}
			}},
			{2}
		});
		nested.f_replace(2, 1, {
			{3}
		});
		f_assert_equals(nested, {
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
