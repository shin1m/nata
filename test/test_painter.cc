#include "test_rows.h"
#include <nata/painter.h>
#include <string>

int main(int argc, char* argv[])
{
	using namespace std::literals;
	auto setup = [](auto test)
	{
		nata::t_text<nata::t_lines<5, 5>, 5, 5> text;
		nata::t_tokens<decltype(text), int, 5, 5> tokens(text);
		nata::t_painter<decltype(tokens)> painter(tokens);
		test(text, tokens, painter);
	};
	setup([](auto& text, auto& tokens, auto& painter)
	{
		auto s = L"Hello."s;
		text.f_replace(0, 0, s.begin(), s.end());
		painter.f_push(0, 7);
		painter.f_flush();
		f_assert_spans(tokens, {
			{0, 7}
		});
		painter.f_push(0, 0);
		painter.f_flush();
		f_assert_spans(tokens, {
			{0, 7}
		});
	});
	setup([](auto& text, auto& tokens, auto& painter)
	{
		auto s = L"Hello, world!\nGood bye."s;
		text.f_replace(0, 0, s.begin(), s.end());
		painter.f_push(0, 7);
		painter.f_push(1, 5);
		painter.f_push(0, 11);
		painter.f_flush();
		f_assert_spans(tokens, {
			{0, 7},
			{1, 5},
			{0, 12}
		});
		painter.f_reset();
		painter.f_push(0, 7);
		painter.f_push(1, 6);
		painter.f_push(0, 6);
		painter.f_push(1, 3);
		painter.f_push(0, 1);
		painter.f_flush();
		f_assert_spans(tokens, {
			{0, 7},
			{1, 6},
			{0, 6},
			{1, 3},
			{0, 2}
		});
	});
	auto assert_painted = [&](auto& tokens, auto action, std::initializer_list<std::tuple<size_t, size_t>> xs)
	{
		std::vector<std::tuple<size_t, size_t>> ys;
		nata::t_slot<size_t, size_t> painted = [&](auto p, auto n)
		{
			ys.emplace_back(p, n);
		};
		tokens.v_painted >> painted;
		action();
		assert(std::equal(xs.begin(), xs.end(), ys.begin(), ys.end()));
	};
	setup([&](auto& text, auto& tokens, auto& painter)
	{
		auto s = L"xxyyyxx"s;
		text.f_replace(0, 0, s.begin(), s.end());
		tokens.f_paint(2, {
			{1, 3}
		});
		assert_painted(tokens, [&]
		{
			painter.f_push(0, 2);
			painter.f_push(1, 3);
			painter.f_push(0, 2);
			painter.f_flush();
		}, {});
	});
	setup([&](auto& text, auto& tokens, auto& painter)
	{
		auto s = L"xxyyyxx"s;
		text.f_replace(0, 0, s.begin(), s.end());
		tokens.f_paint(2, {
			{1, 3}
		});
		assert_painted(tokens, [&]
		{
			painter.f_push(0, 7);
		}, {{2, 3}});
		assert_painted(tokens, [&]
		{
			painter.f_flush();
		}, {});
	});
	setup([&](auto& text, auto& tokens, auto& painter)
	{
		auto s = L"xxyyyxx"s;
		text.f_replace(0, 0, s.begin(), s.end());
		assert_painted(tokens, [&]
		{
			painter.f_push(0, 2);
			painter.f_push(1, 3);
		}, {});
		assert_painted(tokens, [&]
		{
			painter.f_push(0, 2);
		}, {{2, 3}});
		assert_painted(tokens, [&]
		{
			painter.f_flush();
		}, {});
	});
	setup([&](auto& text, auto& tokens, auto& painter)
	{
		auto s = L"xxyyxyyxx"s;
		text.f_replace(0, 0, s.begin(), s.end());
		assert_painted(tokens, [&]
		{
			painter.f_push(0, 2);
			painter.f_push(1, 2);
		}, {});
		assert_painted(tokens, [&]
		{
			painter.f_push(0, 1);
		}, {{2, 2}});
		assert_painted(tokens, [&]
		{
			painter.f_push(1, 2);
		}, {});
		assert_painted(tokens, [&]
		{
			painter.f_push(0, 2);
		}, {{5, 2}});
		assert_painted(tokens, [&]
		{
			painter.f_flush();
		}, {});
	});
	setup([&](auto& text, auto& tokens, auto& painter)
	{
		auto s = L"xxyyxyyxx"s;
		text.f_replace(0, 0, s.begin(), s.end());
		assert_painted(tokens, [&]
		{
			painter.f_push(0, 2, 1);
			painter.f_push(1, 2, 1);
			painter.f_push(0, 1, 1);
			painter.f_push(1, 2, 1);
		}, {});
		assert_painted(tokens, [&]
		{
			painter.f_push(0, 2, 1);
		}, {{2, 5}});
		assert_painted(tokens, [&]
		{
			painter.f_flush();
		}, {});
	});
	setup([&](auto& text, auto& tokens, auto& painter)
	{
		auto s = L"xyyzyyx"s;
		text.f_replace(0, 0, s.begin(), s.end());
		assert_painted(tokens, [&]
		{
			painter.f_push(0, 1);
			painter.f_push(1, 2);
			painter.f_push(2, 1);
			painter.f_push(1, 2);
		}, {});
		assert_painted(tokens, [&]
		{
			painter.f_push(0, 1);
		}, {{1, 5}});
		assert_painted(tokens, [&]
		{
			painter.f_flush();
		}, {});
	});
	setup([&](auto& text, auto& tokens, auto& painter)
	{
		auto s = L"xxyyyxx"s;
		text.f_replace(0, 0, s.begin(), s.end());
		assert_painted(tokens, [&]
		{
			painter.f_push(0, 1);
			painter.f_push(0, 1);
			painter.f_push(1, 1);
			painter.f_push(1, 1);
			painter.f_push(1, 1);
		}, {});
		assert_painted(tokens, [&]
		{
			painter.f_push(0, 1);
		}, {{2, 3}});
		assert_painted(tokens, [&]
		{
			painter.f_push(0, 1);
			painter.f_flush();
		}, {});
	});
	return 0;
}
