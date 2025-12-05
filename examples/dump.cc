#include <nata/model.h>
#include <nata/tokens.h>
#include <nata/rows.h>
#include <string>

int main(int argc, char* argv[])
{
	using namespace std::literals;
	nata::t_text<nata::t_lines<5, 5>, 5, 5> text;
	nata::t_tokens<decltype(text), int, 5, 5> tokens(text);
	struct
	{
		nata::t_signal<> v_resized;

		size_t f_width() const
		{
			return 10;
		}
		std::tuple<size_t, size_t, size_t> f_size(wchar_t a_c, int a_a) const
		{
			return {1 + a_a, 2, a_a};
		}
		std::tuple<size_t, size_t, size_t> f_tab(size_t a_x, int a_a) const
		{
			return {(a_x + 8) / 8 * 8 - a_x, 2, 1};
		}
		std::tuple<size_t, size_t, size_t> f_eol(int a_a) const
		{
			return {1, 2, a_a};
		}
		std::tuple<size_t, size_t, size_t> f_eof() const
		{
			return {1, 1, 0};
		}
		std::tuple<size_t, size_t, size_t> f_folded() const
		{
			return {3, 1, 0};
		}
	} target;
	nata::t_rows<decltype(tokens), decltype(target), nata::t_creased<5, 5>, 5, 5> rows(tokens, target);
	{
		auto s = L"Hello,\tworld!!\nGood bye."sv;
		text.f_replace(0, 0, s.begin(), s.end());
		tokens.f_paint(7, {
			{1, 7},
			{0, 6},
			{1, 3}
		});
	}
	{
		auto row = rows.f_at_y(0);
		auto text = rows.v_tokens.v_text.f_at(row.f_index().v_text);
		auto token = rows.v_tokens.f_at_text(text.f_index());
		size_t delta = token.f_index().v_i1 + token.f_delta().v_i1 - text.f_index();
		while (true) {
			auto rd = row.f_delta();
			if (row->v_tail) --rd.v_text;
			std::wprintf(L"/%d\n", row->v_ascent);
			while (rd.v_text > 0) {
				size_t td = std::min(delta, rd.v_text);
				std::wprintf(L"<%d|", token->v_x);
				for (size_t i = 0; i < td; ++i) {
					wchar_t c = *text;
					if (c == L'\t') {
						std::wprintf(L"<TAB>");
					} else {
						std::putwchar(c);
						if (token->v_x != 0) std::putwchar(L'_');
					}
					++text;
				}
				std::putwchar(L'>');
				delta -= td;
				if (delta <= 0) delta = (++token).f_delta().v_i1;
				rd.v_text -= td;
			}
			size_t ascent = rd.v_y - row->v_ascent;
			if (row->v_tail) {
				if (++row == rows.f_end()) {
					std::wprintf(L"<EOF>\n %d/\n", ascent);
					break;
				}
				std::wprintf(L"<%d|EOL>", token->v_x);
				++text;
				if (--delta <= 0) delta = (++token).f_delta().v_i1;
			} else {
				++row;
			}
			std::wprintf(L"\n %d/\n", ascent);
		}
	}
	return 0;
}
