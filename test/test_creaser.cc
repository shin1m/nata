#include "test_nested.h"
#include "test_rows.h"
#include <nata/creaser.h>

int main(int argc, char* argv[])
{
	using namespace std::literals;
	auto setup = [](auto test)
	{
		nata::t_text<nata::t_lines<5, 5>, 5, 5> text;
		nata::t_tokens<decltype(text), int, 5, 5> tokens(text);
		t_test_target target;
		nata::t_rows<decltype(tokens), decltype(target), nata::t_creased<5, 5>, 5, 5> rows(tokens, target);
		nata::t_creaser<decltype(rows)> creaser(rows);
		test(text, rows, creaser);
	};
	setup([](auto& text, auto& rows, auto& creaser)
	{
		auto s = L"Hello,\tworld!\nGood bye."sv;
		text.f_replace(0, 0, s.begin(), s.end());
		creaser.f_end();
		f_assert_nested(rows.f_creases(), {
			{24}
		});
		creaser.f_end();
		f_assert_nested(rows.f_creases(), {
			{24}
		});
	});
	setup([](auto& text, auto& rows, auto& creaser)
	{
		auto s = L"Hello,\tworld!\nGood bye."sv;
		text.f_replace(0, 0, s.begin(), s.end());
		creaser.f_push(7, 11);
		creaser.f_end();
		f_assert_nested(rows.f_creases(), {
			{7},
			{{{11}}},
			{6}
		});
	});
	setup([](auto& text, auto& rows, auto& creaser)
	{
		auto s = L"xx(xx\nxx)(xxxxxxxx)x"sv;
		text.f_replace(0, 0, s.begin(), s.end());
		creaser.f_push(2, 7);
		creaser.f_push(7, 10);
		creaser.f_end();
		f_assert_nested(rows.f_creases(), {
			{2},
			{{{7}}},
			{{{10}}},
			{2}
		});
	});
	setup([](auto& text, auto& rows, auto& creaser)
	{
		auto s = L"xx(xx\nxx)(xxxxxxxx)x"sv;
		text.f_replace(0, 0, s.begin(), s.end());
		creaser.f_push(1, 18);
		creaser.f_push(1, 7);
		creaser.f_push(7, 10);
		creaser.f_end();
		f_assert_nested(rows.f_creases(), {
			{1},
			{{
				{1},
				{{{7}}},
				{{{10}}}
			}},
			{2}
		});
	});
	setup([](auto& text, auto& rows, auto& creaser)
	{
		auto s = L"xx(xx\nxx)(xxxxxxxx)x"sv;
		text.f_replace(0, 0, s.begin(), s.end());
		creaser.f_push(2, 7);
		creaser.f_push(7, 10);
		creaser.f_push(1, 5);
		creaser.f_push(5, 4);
		creaser.f_push(2, 2);
		creaser.f_end();
		f_assert_nested(rows.f_creases(), {
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
	setup([&](auto& text, auto& rows, auto& creaser)
	{
		auto s = L"xxxx"sv;
		text.f_replace(0, 0, s.begin(), s.end());
		assert_not_painted(rows, [&]
		{
			creaser.f_end();
		});
		f_assert_nested(rows.f_creases(), {
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
	setup([&](auto& text, auto& rows, auto& creaser)
	{
		auto s = L"x(x)x"sv;
		text.f_replace(0, 0, s.begin(), s.end());
		rows.f_crease(1, {
			{{{3}}}
		});
		rows.f_folded(1, true);
		assert_painted(rows, [&]
		{
			creaser.f_end();
		}, 1, 3);
		f_assert_nested(rows.f_creases(), {
			{6}
		});
	});
	setup([&](auto& text, auto& rows, auto& creaser)
	{
		auto s = L"x(x)x"sv;
		text.f_replace(0, 0, s.begin(), s.end());
		rows.f_crease(1, {
			{{{3}}}
		});
		rows.f_folded(1, true);
		assert_painted(rows, [&]
		{
			creaser.f_push(0, 3);
		}, 1, 3);
		assert_not_painted(rows, [&]
		{
			creaser.f_end();
		});
		f_assert_nested(rows.f_creases(), {
			{{{3}}},
			{3}
		});
	});
	setup([&](auto& text, auto& rows, auto& creaser)
	{
		auto s = L"x(x)x"sv;
		text.f_replace(0, 0, s.begin(), s.end());
		rows.f_crease(1, {
			{{{3}}}
		});
		rows.f_folded(1, true);
		assert_painted(rows, [&]
		{
			creaser.f_push(2, 3);
		}, 1, 3);
		assert_not_painted(rows, [&]
		{
			creaser.f_end();
		});
		f_assert_nested(rows.f_creases(), {
			{2},
			{{{3}}},
			{1}
		});
	});
	setup([&](auto& text, auto& rows, auto& creaser)
	{
		auto s = L"x(x)x"sv;
		text.f_replace(0, 0, s.begin(), s.end());
		rows.f_crease(1, {
			{{{3}}}
		});
		rows.f_folded(1, true);
		assert_painted(rows, [&]
		{
			creaser.f_push(2, 1);
		}, 1, 3);
		assert_not_painted(rows, [&]
		{
			creaser.f_end();
		});
		f_assert_nested(rows.f_creases(), {
			{2},
			{{{1}}},
			{3}
		});
	});
	return 0;
}
