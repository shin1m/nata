#include "model.h"
#include "test_spans.h"

int main(int argc, char* argv[])
{
	{
		nata::t_lines<5, 5> lines;
		f_assert_spans(lines, {
			{1}
		});
		std::wstring s = L"Hello.";
		lines.f_replace(0, 0, s.begin(), s.end());
		f_assert_spans(lines, {
			{7}
		});
	}
	{
		nata::t_lines<5, 5> lines;
		std::wstring s = L"Hello.\nGood bye.";
		lines.f_replace(0, 0, s.begin(), s.end());
		f_assert_spans(lines, {
			{7},
			{10}
		});
	}
	{
		nata::t_lines<5, 5> lines;
		{
			std::wstring s = L"Hello.\nBye.";
			lines.f_replace(0, 0, s.begin(), s.end());
		}
		{
			std::wstring s = L", world!\nGood b";
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
			std::wstring s = L"\n\n";
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
			std::wstring s = L"Hello.";
			lines.f_replace(0, 0, s.begin(), s.end());
		}
		{
			std::wstring s = L"\nBye.";
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
			std::wstring s = L"ello.\nBye.";
			lines.f_replace(0, 0, s.begin(), s.end());
		}
		{
			std::wstring s = L"H";
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
			std::wstring s = L"Hello.Bye.";
			lines.f_replace(0, 0, s.begin(), s.end());
		}
		{
			std::wstring s = L"\n";
			lines.f_replace(6, 0, s.begin(), s.end());
		}
		f_assert_spans(lines, {
			{7},
			{5}
		});
	}
	return 0;
}
