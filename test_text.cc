#include "model.h"
#include "test_spans.h"

template<typename T_lines, size_t A_leaf, size_t A_branch, typename T>
void f_assert_equals(const nata::t_text<T_lines, A_leaf, A_branch>& a0, const T& a1)
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
void f_assert_equals(const nata::t_text<T_lines, A_leaf, A_branch>& a0, std::initializer_list<std::wstring> a1)
{
	f_assert_equals<T_lines, A_leaf, A_branch, std::initializer_list<std::wstring>>(a0, a1);
}

int main(int argc, char* argv[])
{
	{
		nata::t_text<nata::t_lines<5, 5>, 5, 5> text;
		std::wstring s = L"Hello.";
		text.f_replace(0, 0, s.begin(), s.end());
		f_assert_equals(text, {
			L"Hello."
		});
	}
	{
		nata::t_text<nata::t_lines<5, 5>, 5, 5> text;
		std::wstring s = L"Hello.\nGood bye.";
		text.f_replace(0, 0, s.begin(), s.end());
		f_assert_equals(text, {
			L"Hello.\n",
			L"Good bye."
		});
	}
	{
		nata::t_text<nata::t_lines<5, 5>, 5, 5> text;
		{
			std::wstring s = L"Hello.\nBye.";
			text.f_replace(0, 0, s.begin(), s.end());
		}
		f_assert_equals(text, {
			L"Hello.\n",
			L"Bye."
		});
		{
			std::wstring s = L", world!\nGood b";
			text.f_replace(5, 3, s.begin(), s.end());
		}
		f_assert_equals(text, {
			L"Hello, world!\n",
			L"Good bye."
		});
	}
	{
		nata::t_text<nata::t_lines<5, 5>, 5, 5> text;
		{
			std::wstring s = L"\n\n";
			text.f_replace(0, 0, s.begin(), s.end());
		}
		f_assert_equals(text, {
			L"\n",
			L"\n",
			L""
		});
		{
			std::wstring s;
			text.f_replace(1, 1, s.begin(), s.end());
			f_assert_equals(text, {
				L"\n",
				L""
			});
			text.f_replace(0, 1, s.begin(), s.end());
			f_assert_equals(text, {
				L""
			});
		}
	}
	return 0;
}
