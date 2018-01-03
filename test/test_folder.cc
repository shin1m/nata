#include "test_nested.h"
#include "test_rows.h"
#include "folder.h"

int main(int argc, char* argv[])
{
	auto setup = [](auto test)
	{
		nata::t_text<nata::t_lines<5, 5>, 5, 5> text;
		nata::t_tokens<decltype(text), int, 5, 5> tokens(text);
		t_test_target target;
		nata::t_rows<decltype(tokens), decltype(target), nata::t_foldable<5, 5>, 5, 5> rows(tokens, target);
		nata::t_folder<decltype(rows), int> folder(rows);
		test(text, rows, folder);
	};
	setup([](auto& text, auto& rows, auto& folder)
	{
		std::wstring s = L"Hello,\tworld!\nGood bye.";
		text.f_replace(0, 0, s.begin(), s.end());
		folder.f_reset();
		folder.f_push(7);
		folder.f_open(0);
			folder.f_push(11);
		folder.f_close();
		folder.f_push(5);
		folder.f_flush();
		f_assert_equals(rows.f_foldings(), {
			{7},
			{{{11}}},
			{6}
		});
	});
	setup([](auto& text, auto& rows, auto& folder)
	{
		std::wstring s = L"xx(xx\nxx)(xxxxxxxx)x";
		text.f_replace(0, 0, s.begin(), s.end());
		folder.f_reset();
		folder.f_push(2);
		folder.f_open(0);
			folder.f_push(7);
		folder.f_close();
		folder.f_open(0);
			folder.f_push(10);
		folder.f_close();
		folder.f_push(1);
		folder.f_flush();
		f_assert_equals(rows.f_foldings(), {
			{2},
			{{{7}}},
			{{{10}}},
			{2}
		});
	});
	setup([](auto& text, auto& rows, auto& folder)
	{
		std::wstring s = L"xx(xx\nxx)(xxxxxxxx)x";
		text.f_replace(0, 0, s.begin(), s.end());
		folder.f_reset();
		folder.f_push(1);
		folder.f_open(0);
			folder.f_push(1);
			folder.f_open(0);
				folder.f_push(7);
			folder.f_close();
			folder.f_open(0);
				folder.f_push(10);
			folder.f_close();
		folder.f_close();
		folder.f_push(1);
		folder.f_flush();
		f_assert_equals(rows.f_foldings(), {
			{1},
			{{
				{1},
				{{{7}}},
				{{{10}}}
			}},
			{2}
		});
	});
	setup([](auto& text, auto& rows, auto& folder)
	{
		std::wstring s = L"xx(xx\nxx)(xxxxxxxx)x";
		text.f_replace(0, 0, s.begin(), s.end());
		folder.f_reset();
		folder.f_push(2);
		folder.f_open(0);
			folder.f_push(7);
		folder.f_close();
		folder.f_open(0);
			folder.f_push(1);
			folder.f_open(0);
				folder.f_push(5);
			folder.f_close();
			folder.f_open(0);
				folder.f_push(2);
				folder.f_open(0);
					folder.f_push(2);
				folder.f_close();
			folder.f_close();
		folder.f_close();
		folder.f_push(1);
		folder.f_flush();
		f_assert_equals(rows.f_foldings(), {
			{2},
			{{{7}}},
			{{
				{1},
				{{{5}}},
				{{
					{2},
					{{{2}}}
				}}
			}},
			{2}
		});
	});
	auto assert_not_painted = [&](auto& rows, auto action)
	{
		bool called = false;
		nata::t_slot<size_t, size_t, size_t, size_t, size_t> painted = [&](auto, auto, auto, auto, auto)
		{
			called = true;
		};
		rows.v_painted >> painted;
		action();
		assert(!called);
	};
	setup([&](auto& text, auto& rows, auto& folder)
	{
		std::wstring s = L"xxxx";
		text.f_replace(0, 0, s.begin(), s.end());
		folder.f_reset();
		folder.f_push(4);
		assert_not_painted(rows, [&]
		{
			folder.f_flush();
		});
		f_assert_equals(rows.f_foldings(), {
			{5}
		});
	});
	auto assert_painted = [&](auto& rows, auto action, size_t a_p, size_t a_n)
	{
		bool called = false;
		nata::t_slot<size_t, size_t, size_t, size_t, size_t> painted = [&](auto p, auto n, auto, auto, auto)
		{
			called = true;
			assert(p == a_p);
			assert(n == a_n);
		};
		rows.v_painted >> painted;
		action();
		assert(called);
	};
	setup([&](auto& text, auto& rows, auto& folder)
	{
		std::wstring s = L"x(x)x";
		text.f_replace(0, 0, s.begin(), s.end());
		rows.f_foldable(1, {
			{{{3}}}
		});
		rows.f_folded(1, true);
		folder.f_reset();
		folder.f_push(5);
		assert_painted(rows, [&]
		{
			folder.f_flush();
		}, 1, 3);
		f_assert_equals(rows.f_foldings(), {
			{6}
		});
	});
	setup([&](auto& text, auto& rows, auto& folder)
	{
		std::wstring s = L"x(x)x";
		text.f_replace(0, 0, s.begin(), s.end());
		rows.f_foldable(1, {
			{{{3}}}
		});
		rows.f_folded(1, true);
		folder.f_reset();
		folder.f_open(0);
		folder.f_push(3);
		assert_painted(rows, [&]
		{
			folder.f_close();
		}, 1, 2);
		folder.f_push(2);
		assert_not_painted(rows, [&]
		{
			folder.f_flush();
		});
		f_assert_equals(rows.f_foldings(), {
			{{{3}}},
			{3}
		});
	});
	setup([&](auto& text, auto& rows, auto& folder)
	{
		std::wstring s = L"x(x)x";
		text.f_replace(0, 0, s.begin(), s.end());
		rows.f_foldable(1, {
			{{{3}}}
		});
		rows.f_folded(1, true);
		folder.f_reset();
		folder.f_push(2);
		folder.f_open(0);
		folder.f_push(3);
		assert_painted(rows, [&]
		{
			folder.f_close();
		}, 1, 3);
		assert_not_painted(rows, [&]
		{
			folder.f_flush();
		});
		f_assert_equals(rows.f_foldings(), {
			{2},
			{{{3}}},
			{1}
		});
	});
	setup([&](auto& text, auto& rows, auto& folder)
	{
		std::wstring s = L"x(x)x";
		text.f_replace(0, 0, s.begin(), s.end());
		rows.f_foldable(1, {
			{{{3}}}
		});
		rows.f_folded(1, true);
		folder.f_reset();
		folder.f_push(2);
		folder.f_open(0);
		folder.f_push(1);
		assert_painted(rows, [&]
		{
			folder.f_close();
		}, 1, 2);
		folder.f_push(2);
		assert_not_painted(rows, [&]
		{
			folder.f_flush();
		});
		f_assert_equals(rows.f_foldings(), {
			{2},
			{{{1}}},
			{3}
		});
	});
	return 0;
}
