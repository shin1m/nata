#include "test_spans.h"
#include <nata/model.h>
#include <nata/tokens.h>
#include <string>

int main(int argc, char* argv[])
{
	using namespace std::literals;
	auto setup = [](auto test)
	{
		nata::t_text<nata::t_lines<5, 5>, 5, 5> text;
		nata::t_tokens<decltype(text), int, 5, 5> tokens(text);
		test(text, tokens);
	};
	setup([](auto& text, auto& tokens)
	{
		auto s = L"Hello."s;
		text.f_replace(0, 0, s.begin(), s.end());
		f_assert_spans(tokens, {
			{0, 7}
		});
	});
	setup([](auto& text, auto& tokens)
	{
		auto s = L"Hello, world!\nGood bye."s;
		text.f_replace(0, 0, s.begin(), s.end());
		tokens.f_paint(7, {
			{1, 5}
		});
		f_assert_spans(tokens, {
			{0, 7},
			{1, 5},
			{0, 12}
		});
		tokens.f_paint(12, {
			{1, 1},
			{0, 6},
			{1, 3}
		});
		f_assert_spans(tokens, {
			{0, 7},
			{1, 6},
			{0, 6},
			{1, 3},
			{0, 2}
		});
	});
	return 0;
}
