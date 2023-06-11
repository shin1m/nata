#include <nata/model.h>
#include "test_spans.h"
#include <string>

using namespace std::literals;

template<typename T_lines, size_t A_leaf, size_t A_branch>
void f_assert_equals(const nata::t_text<T_lines, A_leaf, A_branch>& a0, const auto& a1)
{
	assert(a0.f_lines().f_size().v_i0 == a1.size());
	auto i = a0.f_lines().f_begin();
	for (auto s : a1) {
		auto i0 = a0.f_at(i.f_index().v_i1);
		auto i1 = a0.f_at((++i).f_index().v_i1);
		assert(std::equal(i0, i1, s.begin(), s.end()));
	}
}

template<typename T_lines, size_t A_leaf, size_t A_branch>
void f_assert_equals(const nata::t_text<T_lines, A_leaf, A_branch>& a0, std::initializer_list<std::wstring_view> a1)
{
	f_assert_equals<T_lines, A_leaf, A_branch, std::initializer_list<std::wstring_view>>(a0, a1);
}

int main(int argc, char* argv[])
{
	using namespace std::literals;
	{
		nata::t_text<nata::t_lines<5, 5>, 5, 5> text;
		auto s = L"Hello."s;
		text.f_replace(0, 0, s.begin(), s.end());
		f_assert_equals(text, {
			L"Hello."sv
		});
	}
	{
		nata::t_text<nata::t_lines<5, 5>, 5, 5> text;
		auto s = L"Hello.\nGood bye."s;
		text.f_replace(0, 0, s.begin(), s.end());
		f_assert_equals(text, {
			L"Hello.\n"sv,
			L"Good bye."sv
		});
	}
	{
		nata::t_text<nata::t_lines<5, 5>, 5, 5> text;
		{
			auto s = L"Hello.\nBye."s;
			text.f_replace(0, 0, s.begin(), s.end());
		}
		f_assert_equals(text, {
			L"Hello.\n"sv,
			L"Bye."sv
		});
		{
			auto s = L", world!\nGood b"s;
			text.f_replace(5, 3, s.begin(), s.end());
		}
		f_assert_equals(text, {
			L"Hello, world!\n"sv,
			L"Good bye."sv
		});
	}
	{
		nata::t_text<nata::t_lines<5, 5>, 5, 5> text;
		{
			auto s = L"\n\n"s;
			text.f_replace(0, 0, s.begin(), s.end());
		}
		f_assert_equals(text, {
			L"\n"sv,
			L"\n"sv,
			L""sv
		});
		{
			auto s = L""s;
			text.f_replace(1, 1, s.begin(), s.end());
			f_assert_equals(text, {
				L"\n"sv,
				L""sv
			});
			text.f_replace(0, 1, s.begin(), s.end());
			f_assert_equals(text, {
				L""sv
			});
		}
	}
	return 0;
}
