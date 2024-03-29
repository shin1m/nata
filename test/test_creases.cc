#include "test_rows.h"
#include <string>

int main(int argc, char* argv[])
{
	using namespace std::literals;
	auto setup = [](auto test)
	{
		nata::t_text<nata::t_lines<5, 5>, 5, 5> text;
		nata::t_tokens<decltype(text), int, 5, 5> tokens(text);
		t_test_target target;
		nata::t_rows<decltype(tokens), decltype(target), nata::t_creased<5, 5>, 5, 5> rows(tokens, target);
		test(text, rows);
	};
	setup([](auto& text, auto& rows)
	{
		auto s = L"Hello,\tworld!\nGood bye."sv;
		text.f_replace(0, 0, s.begin(), s.end());
		rows.f_crease(7, {
			{{{11}}}
		});
		f_assert_rows(rows, {
			{1, false, 9, 10, 2, 3},
			{0, true, 5, 5, 2, 2},
			{1, true, 10, 10, 2, 2}
		});
		rows.f_folded(7, true);
		f_assert_rows(rows, {
			{1, false, 7, 8, 2, 3},
			{0, true, 17, 9, 2, 2}
		});
	});
	setup([](auto& text, auto& rows)
	{
		auto s = L"x"sv;
		text.f_replace(0, 0, s.begin(), s.end());
		rows.f_crease(0, {
			{{{1}}}
		});
		f_assert_rows(rows, {
			{1, true, 2, 2, 2, 2}
		});
		rows.f_folded(0, true);
		f_assert_rows(rows, {
			{1, true, 2, 4, 1, 1}
		});
	});
	setup([](auto& text, auto& rows)
	{
		auto s = L"xx(xx\nxx)(xxxxxxxx)x"sv;
		text.f_replace(0, 0, s.begin(), s.end());
		rows.f_crease(2, {
			{{{7}}},
			{{{10}}}
		});
		f_assert_rows(rows, {
			{1, true, 6, 6, 2, 2},
			{1, false, 10, 10, 2, 2},
			{0, true, 5, 5, 2, 2}
		});
		rows.f_folded(9, true);
		f_assert_rows(rows, {
			{1, true, 6, 6, 2, 2},
			{1, true, 15, 8, 2, 2}
		});
		rows.f_folded(2, true);
		f_assert_rows(rows, {
			{1, true, 21, 10, 2, 2}
		});
		rows.f_folded(9, false);
		f_assert_rows(rows, {
			{1, false, 14, 10, 2, 2},
			{0, true, 7, 7, 2, 2}
		});
		rows.f_folded(2, false);
		f_assert_rows(rows, {
			{1, true, 6, 6, 2, 2},
			{1, false, 10, 10, 2, 2},
			{0, true, 5, 5, 2, 2}
		});
	});
	setup([](auto& text, auto& rows)
	{
		auto s = L"xx(xx\nxx)(xxxxxxxx)x"sv;
		text.f_replace(0, 0, s.begin(), s.end());
		rows.f_crease(1, {
			{{
				{1},
				{{{7}}},
				{{{10}}}
			}}
		});
		f_assert_rows(rows, {
			{1, true, 6, 6, 2, 2},
			{1, false, 10, 10, 2, 2},
			{0, true, 5, 5, 2, 2}
		});
		rows.f_folded(9, true);
		f_assert_rows(rows, {
			{1, true, 6, 6, 2, 2},
			{1, true, 15, 8, 2, 2}
		});
		rows.f_folded(2, true);
		f_assert_rows(rows, {
			{1, true, 21, 10, 2, 2}
		});
		rows.f_folded(9, false);
		f_assert_rows(rows, {
			{1, false, 14, 10, 2, 2},
			{0, true, 7, 7, 2, 2}
		});
		rows.f_folded(2, false);
		f_assert_rows(rows, {
			{1, true, 6, 6, 2, 2},
			{1, false, 10, 10, 2, 2},
			{0, true, 5, 5, 2, 2}
		});
	});
	setup([](auto& text, auto& rows)
	{
		auto s = L"xx(xx\nxx)(xxxxxxxx)x"sv;
		text.f_replace(0, 0, s.begin(), s.end());
		rows.f_crease(2, {
			{{{7}}},
			{{
				{1},
				{{{5}}},
				{{
					{2},
					{{{2}}}
				}}
			}}
		});
		f_assert_rows(rows, {
			{1, true, 6, 6, 2, 2},
			{1, false, 10, 10, 2, 2},
			{0, true, 5, 5, 2, 2}
		});
	});
	setup([](auto& text, auto& rows)
	{
		auto s = L"x"sv;
		text.f_replace(0, 0, s.begin(), s.end());
		rows.f_crease(0, {
			{{{1}}}
		});
		{
			std::vector<std::tuple<size_t, size_t, size_t>> xs;
			assert(rows.f_each_x(rows.f_begin(), [&](size_t p, size_t x, size_t width)
			{
				xs.push_back({p, x, width});
				return true;
			}) == std::make_tuple(1, 1, 1));
			assert(xs == decltype(xs)({
				{0, 0, 1}
			}));
		}
		rows.f_folded(0, true);
		{
			std::vector<std::tuple<size_t, size_t, size_t>> xs;
			assert(rows.f_each_x(rows.f_at(0), [&](size_t p, size_t x, size_t width)
			{
				xs.push_back({p, x, width});
				return true;
			}) == std::make_tuple(1, 3, 1));
			assert(xs == decltype(xs)({
				{0, 0, 3}
			}));
		}
		f_dump(rows);
	});
	setup([](auto& text, auto& rows)
	{
		auto s = L"xx(xx\nxx)(xxxxxxxx)x"sv;
		text.f_replace(0, 0, s.begin(), s.end());
		rows.f_crease(2, {
			{{{7}}},
			{{{10}}}
		});
		{
			std::vector<std::tuple<size_t, size_t, size_t>> xs;
			assert(rows.f_each_x(rows.f_begin(), [&](size_t p, size_t x, size_t width)
			{
				xs.push_back({p, x, width});
				return true;
			}) == std::make_tuple(5, 5, 1));
			assert(xs == decltype(xs)({
				{0, 0, 1},
				{1, 1, 1},
				{2, 2, 1},
				{3, 3, 1},
				{4, 4, 1}
			}));
		}
		{
			std::vector<std::tuple<size_t, size_t, size_t>> xs;
			assert(rows.f_each_x(rows.f_at(1), [&](size_t p, size_t x, size_t width)
			{
				xs.push_back({p, x, width});
				return true;
			}) == std::make_tuple(16, 16, 0));
			assert(xs == decltype(xs)({
				{6, 6, 1},
				{7, 7, 1},
				{8, 8, 1},
				{9, 9, 1},
				{10, 10, 1},
				{11, 11, 1},
				{12, 12, 1},
				{13, 13, 1},
				{14, 14, 1},
				{15, 15, 1}
			}));
		}
		{
			std::vector<std::tuple<size_t, size_t, size_t>> xs;
			assert(rows.f_each_x(rows.f_at(2), [&](size_t p, size_t x, size_t width)
			{
				xs.push_back({p, x, width});
				return true;
			}) == std::make_tuple(20, 20, 1));
			assert(xs == decltype(xs)({
				{16, 16, 1},
				{17, 17, 1},
				{18, 18, 1},
				{19, 19, 1}
			}));
		}
		rows.f_folded(9, true);
		{
			std::vector<std::tuple<size_t, size_t, size_t>> xs;
			assert(rows.f_each_x(rows.f_at(1), [&](size_t p, size_t x, size_t width)
			{
				xs.push_back({p, x, width});
				return true;
			}) == std::make_tuple(20, 13, 1));
			assert(xs == decltype(xs)({
				{6, 6, 1},
				{7, 7, 1},
				{8, 8, 1},
				{9, 9, 3},
				{19, 12, 1}
			}));
		}
		rows.f_folded(2, true);
		{
			std::vector<std::tuple<size_t, size_t, size_t>> xs;
			assert(rows.f_each_x(rows.f_at(0), [&](size_t p, size_t x, size_t width)
			{
				xs.push_back({p, x, width});
				return true;
			}) == std::make_tuple(20, 9, 1));
			assert(xs == decltype(xs)({
				{0, 0, 1},
				{1, 1, 1},
				{2, 2, 3},
				{9, 5, 3},
				{19, 8, 1}
			}));
		}
		f_dump(rows);
	});
	return 0;
}
