#include "test_spans.h"
#include <nata/model.h>
#include <nata/marks.h>
#include <string>

int main(int argc, char* argv[])
{
	using namespace std::literals;
	auto setup = [](auto test)
	{
		nata::t_text<nata::t_lines<5, 5>, 5, 5> text;
		nata::t_marks<decltype(text), int, 5, 5> marks(text);
		test(text, marks);
	};
	setup([](auto& text, auto& marks)
	{
		auto s = L"Hello."s;
		text.f_replace(0, 0, s.begin(), s.end());
		f_assert_spans(marks, {
			{0, 8, 1.0}
		});
		auto i = marks.f_insert(0, 1);
		f_assert_spans(marks, {
			{0, 1, 0.25},
			{1, 0, 0.25},
			{0, 7, 0.5}
		});
		assert(i == marks.f_first_at_text(0));
		assert((i.f_index() == decltype(i.f_index()){1, 1, 0.25}));
		assert((i.f_delta() == decltype(i.f_delta()){1, 0, 0.25}));
		assert(i == marks.f_at_key(0.25));
		i = marks.f_insert(5, 2);
		f_assert_spans(marks, {
			{0, 1, 0.25},
			{1, 0, 0.25},
			{0, 5, 0.125},
			{2, 0, 0.125},
			{0, 2, 0.25}
		});
		assert(i == marks.f_first_at_text(5));
		assert((i.f_index() == decltype(i.f_index()){3, 6, 0.625}));
		assert((i.f_delta() == decltype(i.f_delta()){1, 0, 0.125}));
		assert(i == marks.f_at_key(0.625));
		i = marks.f_insert(5, 3);
		f_assert_spans(marks, {
			{0, 1, 0.25},
			{1, 0, 0.25},
			{0, 5, 0.125},
			{2, 0, 0.0625},
			{3, 0, 0.0625},
			{0, 2, 0.25}
		});
		assert(i == ++marks.f_first_at_text(5));
		assert((i.f_index() == decltype(i.f_index()){4, 6, 0.6875}));
		assert((i.f_delta() == decltype(i.f_delta()){1, 0, 0.0625}));
		assert(i == marks.f_at_key(0.6875));
	});
	setup([](auto& text, auto& marks)
	{
		auto s = L"Hello."s;
		text.f_replace(0, 0, s.begin(), s.end());
		marks.f_insert(0, 1);
		marks.f_insert(5, 2);
		marks.f_insert(5, 3);
		auto i = marks.f_erase(marks.f_last_at_text(0));
		f_assert_spans(marks, {
			{0, 1, 0.25},
			{1, 0, 0.25},
			{0, 5, 0.125},
			{2, 0, 0.0625},
			{3, 0, 0.0625},
			{0, 2, 0.25}
		});
		assert(i == marks.f_last_at_text(0));
		assert((i.f_index() == decltype(i.f_index()){2, 1, 0.5}));
		assert((i.f_delta() == decltype(i.f_delta()){1, 5, 0.125}));
		i = marks.f_erase(marks.f_first_at_text(0));
		f_assert_spans(marks, {
			{0, 6, 0.625},
			{2, 0, 0.0625},
			{3, 0, 0.0625},
			{0, 2, 0.25}
		});
		assert(i == marks.f_first_at_text(5));
		assert((i.f_index() == decltype(i.f_index()){1, 6, 0.625}));
		assert((i.f_delta() == decltype(i.f_delta()){1, 0, 0.0625}));
		i = marks.f_erase(marks.f_first_at_text(5));
		f_assert_spans(marks, {
			{0, 6, 0.6875},
			{3, 0, 0.0625},
			{0, 2, 0.25}
		});
		assert(i == marks.f_first_at_text(5));
		assert((i.f_index() == decltype(i.f_index()){1, 6, 0.6875}));
		assert((i.f_delta() == decltype(i.f_delta()){1, 0, 0.0625}));
		i = marks.f_erase(i);
		f_assert_spans(marks, {
			{0, 8, 1.0}
		});
		assert(i == marks.f_end());
		assert((i.f_index() == decltype(i.f_index()){1, 8, 1.0}));
	});
	setup([](auto& text, auto& marks)
	{
		auto s = L"Hello."s;
		text.f_replace(0, 0, s.begin(), s.end());
		marks.f_insert(0, 1);
		marks.f_insert(5, 2);
		marks.f_insert(5, 3);
		f_assert_spans(marks, {
			{0, 1, 0.25},
			{1, 0, 0.25},
			{0, 5, 0.125},
			{2, 0, 0.0625},
			{3, 0, 0.0625},
			{0, 2, 0.25}
		});
		s = L"Hi, "s;
		text.f_replace(0, 0, s.begin(), s.end());
		f_assert_spans(marks, {
			{0, 5, 0.25},
			{1, 0, 0.25},
			{0, 5, 0.125},
			{2, 0, 0.0625},
			{3, 0, 0.0625},
			{0, 2, 0.25}
		});
		s = L""s;
		text.f_replace(0, 4, s.begin(), s.end());
		f_assert_spans(marks, {
			{0, 1, 0.25},
			{1, 0, 0.25},
			{0, 5, 0.125},
			{2, 0, 0.0625},
			{3, 0, 0.0625},
			{0, 2, 0.25}
		});
		s = L"i, H"s;
		text.f_replace(1, 0, s.begin(), s.end());
		f_assert_spans(marks, {
			{0, 1, 0.25},
			{1, 0, 0.25},
			{0, 9, 0.125},
			{2, 0, 0.0625},
			{3, 0, 0.0625},
			{0, 2, 0.25}
		});
		s = L"ello, World!"s;
		text.f_replace(1, 9, s.begin(), s.end());
		f_assert_spans(marks, {
			{0, 1, 0.25},
			{1, 0, 0.25},
			{0, 14, 0.5}
		});
	});
	return 0;
}
