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
		auto s = L"Hello,\tworld!\nGood bye."s;
		text.f_replace(0, 0, s.begin(), s.end());
		test(text, rows);
	};
	setup([](auto& text, auto& rows)
	{
		f_assert_rows(rows, {
			{1, false, 9, 10, 2, 3},
			{0, true, 5, 5, 2, 2},
			{1, true, 10, 10, 2, 2}
		});
	});
	setup([](auto& text, auto& rows)
	{
		rows.v_target.v_width = 15;
		rows.v_target.v_resized();
		f_assert_rows(rows, {
			{1, true, 14, 15, 2, 3},
			{1, true, 10, 10, 2, 2}
		});
	});
	setup([](auto& text, auto& rows)
	{
		auto i = rows.f_at(1);
		auto j = rows.f_at(2);
		assert(i.f_index() + i.f_delta() == j.f_index());
		assert(j.f_index() - i.f_delta() == i.f_index());
	});
	setup([](auto& text, auto& rows)
	{
		auto s = L"."s;
		text.f_replace(5, 8, s.begin(), s.end());
		f_assert_rows(rows, {
			{1, true, 7, 7, 2, 2},
			{1, true, 10, 10, 2, 2}
		});
	});
	setup([](auto& text, auto& rows)
	{
		auto s = L"Bye."s;
		text.f_replace(14, 9, s.begin(), s.end());
		f_assert_rows(rows, {
			{1, false, 9, 10, 2, 3},
			{0, true, 5, 5, 2, 2},
			{1, true, 5, 5, 2, 2}
		});
	});
	setup([](auto& text, auto& rows)
	{
		auto s = L"Hi"s;
		text.f_replace(0, 5, s.begin(), s.end());
		f_assert_rows(rows, {
			{1, false, 10, 10, 2, 3},
			{0, true, 1, 1, 2, 2},
			{1, true, 10, 10, 2, 2}
		});
	});
	setup([](auto& text, auto& rows)
	{
		auto s = L"Hi "s;
		text.f_replace(0, 7, s.begin(), s.end());
		f_assert_rows(rows, {
			{1, true, 10, 10, 2, 2},
			{1, true, 10, 10, 2, 2}
		});
	});
	setup([](auto& text, auto& rows)
	{
		auto s = L""s;
		text.f_replace(10, 1, s.begin(), s.end());
		f_assert_rows(rows, {
			{1, false, 9, 10, 2, 3},
			{0, true, 4, 4, 2, 2},
			{1, true, 10, 10, 2, 2}
		});
	});
	setup([](auto& text, auto& rows)
	{
		auto s = L"DDDDDDDDD"s;
		text.f_replace(11, 1, s.begin(), s.end());
		f_assert_rows(rows, {
			{1, false, 9, 10, 2, 3},
			{0, false, 10, 10, 2, 2},
			{0, true, 3, 3, 2, 2},
			{1, true, 10, 10, 2, 2}
		});
	});
	setup([](auto& text, auto& rows)
	{
		auto s = L"\t\t"s;
		text.f_replace(18, 1, s.begin(), s.end());
		f_assert_rows(rows, {
			{1, false, 9, 10, 2, 3},
			{0, true, 5, 5, 2, 2},
			{1, false, 5, 8, 2, 3},
			{0, true, 6, 9, 2, 3}
		});
	});
	setup([](auto& text, auto& rows)
	{
		auto s = L"\n"s;
		text.f_replace(0, 0, s.begin(), s.end());
		f_assert_rows(rows, {
			{1, true, 1, 1, 2, 2},
			{1, false, 9, 10, 2, 3},
			{0, true, 5, 5, 2, 2},
			{1, true, 10, 10, 2, 2}
		});
	});
	setup([](auto& text, auto& rows)
	{
		auto s = L" "s;
		text.f_replace(9, 0, s.begin(), s.end());
		f_assert_rows(rows, {
			{1, false, 9, 10, 2, 3},
			{0, true, 6, 6, 2, 2},
			{1, true, 10, 10, 2, 2}
		});
	});
	{
		auto test = [&](size_t a_p, size_t a_n, std::wstring_view a_s, size_t a_y, size_t a_h0, size_t a_h1)
		{
			setup([&](auto& text, auto& rows)
			{
				bool called = false;
				nata::t_slot<size_t, size_t, size_t, size_t, size_t, size_t> replaced = [&](auto p, auto n0, auto n1, auto y, auto h0, auto h1)
				{
					called = true;
					assert(p == a_p);
					assert(n0 == a_n);
					assert(n1 == a_s.size());
					assert(y == a_y);
					assert(h0 == a_h0);
					assert(h1 == a_h1);
				};
				rows.v_replaced >> replaced;
				text.f_replace(a_p, a_n, a_s.begin(), a_s.end());
				assert(called);
			});
		};
		test(6, 1, L" "sv, 0, 5, 4);
		test(9, 1, L"R"sv, 0, 5, 5);
		test(10, 1, L"L"sv, 3, 2, 2);
		test(18, 1, L"\t"sv, 5, 2, 5);
	}
	setup([](auto& text, auto& rows)
	{
		rows.v_tokens.f_paint(7, {
			{1, 5}
		});
		f_assert_rows(rows, {
			{1, false, 8, 10, 2, 3},
			{0, true, 6, 10, 2, 3},
			{1, true, 10, 10, 2, 2}
		});
		rows.v_tokens.f_paint(19, {
			{2, 3}
		});
		f_assert_rows(rows, {
			{1, false, 8, 10, 2, 3},
			{0, true, 6, 10, 2, 3},
			{1, false, 6, 8, 2, 4},
			{0, true, 4, 8, 2, 4}
		});
		{
			auto i = rows.f_at_y(5).f_index();
			assert(i.v_i == 1);
			assert(i.v_text == 8);
			assert(i.v_x == 10);
			assert(i.v_y == 3);
		}
		{
			auto i = rows.f_at_y(10).f_index();
			assert(i.v_i == 3);
			assert(i.v_text == 20);
			assert(i.v_x == 28);
			assert(i.v_y == 10);
		}
	});
	setup([](auto& text, auto& rows)
	{
		std::vector<std::tuple<size_t, size_t, size_t>> xs;
		assert(rows.f_each_x(rows.f_begin(), [&](size_t p, size_t x, size_t width)
		{
			xs.push_back({p, x, width});
			return true;
		}) == std::make_tuple(9, 10, 0));
		assert(xs == decltype(xs)({
			{0, 0, 1},
			{1, 1, 1},
			{2, 2, 1},
			{3, 3, 1},
			{4, 4, 1},
			{5, 5, 1},
			{6, 6, 2},
			{7, 8, 1},
			{8, 9, 1}
		}));
	});
	setup([](auto& text, auto& rows)
	{
		std::vector<std::tuple<size_t, size_t, size_t>> xs;
		assert(rows.f_each_x(rows.f_at(1), [&](size_t p, size_t x, size_t width)
		{
			xs.push_back({p, x, width});
			return true;
		}) == std::make_tuple(13, 14, 1));
		assert(xs == decltype(xs)({
			{9, 10, 1},
			{10, 11, 1},
			{11, 12, 1},
			{12, 13, 1}
		}));
	});
	setup([](auto& text, auto& rows)
	{
		std::vector<std::tuple<size_t, size_t, size_t>> xs;
		assert(rows.f_each_x(rows.f_at(2), [&](size_t p, size_t x, size_t width)
		{
			xs.push_back({p, x, width});
			return true;
		}) == std::make_tuple(23, 24, 1));
		assert(xs == decltype(xs)({
			{14, 15, 1},
			{15, 16, 1},
			{16, 17, 1},
			{17, 18, 1},
			{18, 19, 1},
			{19, 20, 1},
			{20, 21, 1},
			{21, 22, 1},
			{22, 23, 1}
		}));
	});
	setup([](auto& text, auto& rows)
	{
		assert(rows.f_each_x(rows.f_at(0), [&](size_t p, size_t x, size_t width)
		{
			return x + width <= 7;
		}) == std::make_tuple(6, 6, 2));
		assert(rows.f_each_x(rows.f_at(2), [&](size_t p, size_t x, size_t width)
		{
			return false;
		}) == std::make_tuple(14, 15, 1));
	});
	return 0;
}
