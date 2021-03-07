#include <nata/model.h>
#include "test_spans.h"
#include <string>

int main(int argc, char* argv[])
{
	using namespace std::literals;
	{
		nata::t_lines<5, 5> lines;
		f_assert_spans(lines, {
			{1}
		});
		auto s = L"Hello."s;
		lines.f_replace(0, 0, s.begin(), s.end());
		f_assert_spans(lines, {
			{7}
		});
	}
	{
		nata::t_lines<5, 5> lines;
		auto s = L"Hello.\nGood bye."s;
		lines.f_replace(0, 0, s.begin(), s.end());
		f_assert_spans(lines, {
			{7},
			{10}
		});
	}
	{
		nata::t_lines<5, 5> lines;
		{
			auto s = L"Hello.\nBye."s;
			lines.f_replace(0, 0, s.begin(), s.end());
		}
		{
			auto s = L", world!\nGood b"s;
			lines.f_replace(5, 3, s.begin(), s.end());
		}
		f_assert_spans(lines, {
			{14},
			{10}
		});
	}
	{
		nata::t_lines<5, 5> lines;
		{
			auto s = L"\n\n"s;
			lines.f_replace(0, 0, s.begin(), s.end());
		}
		f_assert_spans(lines, {
			{1},
			{1},
			{1}
		});
		{
			std::wstring s;
			lines.f_replace(1, 1, s.begin(), s.end());
			f_assert_spans(lines, {
				{1},
				{1}
			});
			lines.f_replace(0, 1, s.begin(), s.end());
			f_assert_spans(lines, {
				{1}
			});
		}
	}
	{
		nata::t_lines<5, 5> lines;
		{
			auto s = L"Hello."s;
			lines.f_replace(0, 0, s.begin(), s.end());
		}
		{
			auto s = L"\nBye."s;
			lines.f_replace(6, 0, s.begin(), s.end());
		}
		f_assert_spans(lines, {
			{7},
			{5}
		});
	}
	{
		nata::t_lines<5, 5> lines;
		{
			auto s = L"ello.\nBye."s;
			lines.f_replace(0, 0, s.begin(), s.end());
		}
		{
			auto s = L"H"s;
			lines.f_replace(0, 0, s.begin(), s.end());
		}
		f_assert_spans(lines, {
			{7},
			{5}
		});
	}
	{
		nata::t_lines<5, 5> lines;
		{
			auto s = L"Hello.Bye."s;
			lines.f_replace(0, 0, s.begin(), s.end());
		}
		{
			auto s = L"\n"s;
			lines.f_replace(6, 0, s.begin(), s.end());
		}
		f_assert_spans(lines, {
			{7},
			{5}
		});
	}
	return 0;
}
