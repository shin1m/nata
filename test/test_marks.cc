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
			{0, 8, nata::t_fraction::c_ONE}
		});
		auto i = marks.f_insert(0, 1);
		f_assert_spans(marks, {
			{0, 1, nata::t_fraction{{0b0010'0000}}},
			{1, 0, nata::t_fraction{{0b0010'0000}}},
			{0, 7, nata::t_fraction{{0b0100'0000}}}
		});
		assert(i == marks.f_first_at_text(0));
		assert((i.f_index() == decltype(i.f_index()){1, 1, nata::t_fraction{{0b0010'0000}}}));
		assert((i.f_delta() == decltype(i.f_delta()){1, 0, nata::t_fraction{{0b0010'0000}}}));
		assert(i == marks.f_at_key(nata::t_fraction{{0b0010'0000}}));
		i = marks.f_insert(5, 2);
		f_assert_spans(marks, {
			{0, 1, nata::t_fraction{{0b0010'0000}}},
			{1, 0, nata::t_fraction{{0b0010'0000}}},
			{0, 5, nata::t_fraction{{0b0001'0000}}},
			{2, 0, nata::t_fraction{{0b0001'0000}}},
			{0, 2, nata::t_fraction{{0b0010'0000}}}
		});
		assert(i == marks.f_first_at_text(5));
		assert((i.f_index() == decltype(i.f_index()){3, 6, nata::t_fraction{{0b0101'0000}}}));
		assert((i.f_delta() == decltype(i.f_delta()){1, 0, nata::t_fraction{{0b0001'0000}}}));
		assert(i == marks.f_at_key(nata::t_fraction{{0b0101'0000}}));
		i = marks.f_insert(5, 3);
		f_assert_spans(marks, {
			{0, 1, nata::t_fraction{{0b0010'0000}}},
			{1, 0, nata::t_fraction{{0b0010'0000}}},
			{0, 5, nata::t_fraction{{0b0001'0000}}},
			{2, 0, nata::t_fraction{{0b0000'1000}}},
			{3, 0, nata::t_fraction{{0b0000'1000}}},
			{0, 2, nata::t_fraction{{0b0010'0000}}}
		});
		assert(i == ++marks.f_first_at_text(5));
		assert((i.f_index() == decltype(i.f_index()){4, 6, nata::t_fraction{{0b0101'1000}}}));
		assert((i.f_delta() == decltype(i.f_delta()){1, 0, nata::t_fraction{{0b0000'1000}}}));
		assert(i == marks.f_at_key(nata::t_fraction{{0b0101'1000}}));
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
			{0, 1, nata::t_fraction{{0b0010'0000}}},
			{1, 0, nata::t_fraction{{0b0010'0000}}},
			{0, 5, nata::t_fraction{{0b0001'0000}}},
			{2, 0, nata::t_fraction{{0b0000'1000}}},
			{3, 0, nata::t_fraction{{0b0000'1000}}},
			{0, 2, nata::t_fraction{{0b0010'0000}}}
		});
		assert(i == marks.f_last_at_text(0));
		assert((i.f_index() == decltype(i.f_index()){2, 1, nata::t_fraction{{0b0100'0000}}}));
		assert((i.f_delta() == decltype(i.f_delta()){1, 5, nata::t_fraction{{0b0001'0000}}}));
		i = marks.f_erase(marks.f_first_at_text(0));
		f_assert_spans(marks, {
			{0, 6, nata::t_fraction{{0b0101'0000}}},
			{2, 0, nata::t_fraction{{0b0000'1000}}},
			{3, 0, nata::t_fraction{{0b0000'1000}}},
			{0, 2, nata::t_fraction{{0b0010'0000}}}
		});
		assert(i == marks.f_first_at_text(5));
		assert((i.f_index() == decltype(i.f_index()){1, 6, nata::t_fraction{{0b0101'0000}}}));
		assert((i.f_delta() == decltype(i.f_delta()){1, 0, nata::t_fraction{{0b0000'1000}}}));
		i = marks.f_erase(marks.f_first_at_text(5));
		f_assert_spans(marks, {
			{0, 6, nata::t_fraction{{0b0101'1000}}},
			{3, 0, nata::t_fraction{{0b0000'1000}}},
			{0, 2, nata::t_fraction{{0b0010'0000}}}
		});
		assert(i == marks.f_first_at_text(5));
		assert((i.f_index() == decltype(i.f_index()){1, 6, nata::t_fraction{{0b0101'1000}}}));
		assert((i.f_delta() == decltype(i.f_delta()){1, 0, nata::t_fraction{{0b0000'1000}}}));
		i = marks.f_erase(i);
		f_assert_spans(marks, {
			{0, 8, nata::t_fraction::c_ONE}
		});
		assert(i == marks.f_end());
		assert((i.f_index() == decltype(i.f_index()){1, 8, nata::t_fraction::c_ONE}));
	});
	setup([](auto& text, auto& marks)
	{
		auto s = L"Hello."s;
		text.f_replace(0, 0, s.begin(), s.end());
		marks.f_insert(0, 1);
		marks.f_insert(5, 2);
		marks.f_insert(5, 3);
		f_assert_spans(marks, {
			{0, 1, nata::t_fraction{{0b0010'0000}}},
			{1, 0, nata::t_fraction{{0b0010'0000}}},
			{0, 5, nata::t_fraction{{0b0001'0000}}},
			{2, 0, nata::t_fraction{{0b0000'1000}}},
			{3, 0, nata::t_fraction{{0b0000'1000}}},
			{0, 2, nata::t_fraction{{0b0010'0000}}}
		});
		s = L"Hi, "s;
		text.f_replace(0, 0, s.begin(), s.end());
		f_assert_spans(marks, {
			{0, 5, nata::t_fraction{{0b0010'0000}}},
			{1, 0, nata::t_fraction{{0b0010'0000}}},
			{0, 5, nata::t_fraction{{0b0001'0000}}},
			{2, 0, nata::t_fraction{{0b0000'1000}}},
			{3, 0, nata::t_fraction{{0b0000'1000}}},
			{0, 2, nata::t_fraction{{0b0010'0000}}}
		});
		s = L""s;
		text.f_replace(0, 4, s.begin(), s.end());
		f_assert_spans(marks, {
			{0, 1, nata::t_fraction{{0b0010'0000}}},
			{1, 0, nata::t_fraction{{0b0010'0000}}},
			{0, 5, nata::t_fraction{{0b0001'0000}}},
			{2, 0, nata::t_fraction{{0b0000'1000}}},
			{3, 0, nata::t_fraction{{0b0000'1000}}},
			{0, 2, nata::t_fraction{{0b0010'0000}}}
		});
		s = L"i, H"s;
		text.f_replace(1, 0, s.begin(), s.end());
		f_assert_spans(marks, {
			{0, 1, nata::t_fraction{{0b0010'0000}}},
			{1, 0, nata::t_fraction{{0b0010'0000}}},
			{0, 9, nata::t_fraction{{0b0001'0000}}},
			{2, 0, nata::t_fraction{{0b0000'1000}}},
			{3, 0, nata::t_fraction{{0b0000'1000}}},
			{0, 2, nata::t_fraction{{0b0010'0000}}}
		});
		s = L"ello, World!"s;
		text.f_replace(1, 9, s.begin(), s.end());
		f_assert_spans(marks, {
			{0, 1, nata::t_fraction{{0b0010'0000}}},
			{1, 0, nata::t_fraction{{0b0010'0000}}},
			{0, 14, nata::t_fraction{{0b0100'0000}}}
		});
	});
	return 0;
}
