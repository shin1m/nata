#include "view.h"
#include <fstream>
#include <regex>
#include <ncurses.h>

struct t_curses
{
	t_curses()
	{
		std::setlocale(LC_ALL, "");
		initscr();
		start_color();
		use_default_colors();
		raw();
		noecho();
		nonl();
		idlok(stdscr, TRUE);
		scrollok(stdscr, TRUE);
		intrflush(stdscr, FALSE);
		keypad(stdscr, TRUE);
	}
	~t_curses()
	{
		endwin();
	}
};

int main(int argc, char* argv[])
{
	t_curses curses;
	init_pair(1, COLOR_WHITE, -1);
	const attr_t attribute_control = A_DIM | COLOR_PAIR(1);
	init_pair(2, COLOR_BLUE, -1);
	const attr_t attribute_comment = COLOR_PAIR(2);
	init_pair(3, COLOR_YELLOW, -1);
	const attr_t attribute_keyword = COLOR_PAIR(3);
	nata::t_text<nata::t_lines<5, 5>, 5, 5> text;
	nata::t_tokens<decltype(text), attr_t, 5, 5> tokens(text);
	struct
	{
		size_t v_width = 10;
		nata::t_signal<> v_resized;

		size_t f_width() const
		{
			return v_width;
		}
		void f_width__(size_t a_value)
		{
			if (a_value == v_width) return;
			v_width = a_value;
			v_resized();
		}
		std::tuple<size_t, size_t, size_t> f_size(wchar_t a_c, attr_t a_a) const
		{
			return {wcwidth(a_c), 1, 0};
		}
		std::tuple<size_t, size_t, size_t> f_tab(size_t a_x, attr_t a_a) const
		{
			return {std::max(std::min((a_x + 8) / 8 * 8, f_width()) - a_x, size_t(1)), 1, 0};
		}
		std::tuple<size_t, size_t, size_t> f_eol(attr_t a_a) const
		{
			return {1, 1, 0};
		}
		std::tuple<size_t, size_t, size_t> f_eof() const
		{
			return {1, 1, 0};
		}
	} target;
	nata::t_rows<decltype(tokens), decltype(target), 5, 5> rows(tokens, target);
	auto render_rows = [&](size_t y, size_t to, size_t height)
	{
		auto row = rows.f_at_in_y(y);
		auto text = rows.v_tokens.v_text.f_at(row.f_index().v_text);
		auto token = rows.v_tokens.f_at_in_text(text.f_index());
		size_t delta = token.f_index().v_i1 + token.f_delta().v_i1 - text.f_index();
		size_t bottom = to + height;
		to -= y - row.f_index().v_y;
		while (to < bottom) {
			move(to, 0);
			auto rd = row.f_delta();
			if (row->v_tail) --rd.v_text;
			size_t x = 0;
			while (rd.v_text > 0) {
				cchar_t cc{token->v_x};
				cc.chars[1] = L'\0';
				size_t td = std::min(delta, rd.v_text);
				for (size_t i = 0; i < td; ++i) {
					wchar_t c = *text;
					if (c == L'\t') {
						size_t w = std::get<0>(target.f_tab(x, token->v_x));
						if (x % 8 == 0) {
							cchar_t cc{attribute_control, L"|"};
							add_wch(&cc);
							++x;
							--w;
						}
						cc.chars[0] = L' ';
						while (w > 0) {
							add_wch(&cc);
							++x;
							--w;
						}
					} else {
						cc.chars[0] = c;
						add_wch(&cc);
						x += std::get<0>(target.f_size(c, token->v_x));
					}
					++text;
				}
				delta -= td;
				if (delta <= 0) delta = (++token).f_delta().v_i1;
				rd.v_text -= td;
			}
			to += rd.v_y;
			if (row->v_tail) {
				if (++row == rows.f_end()) {
					cchar_t cc{attribute_control, L"<"};
					add_wch(&cc);
					clrtoeol();
					break;
				}
				cchar_t cc{attribute_control, L"/"};
				add_wch(&cc);
				++text;
				if (--delta <= 0) delta = (++token).f_delta().v_i1;
			} else {
				++row;
			}
			clrtoeol();
		}
		while (to < bottom) {
			move(to, 0);
			clrtoeol();
			++to;
		}
	};
	struct
	{
		const decltype(text.f_lines())& v_lines;
		const decltype(rows)& v_rows;
		size_t v_height = 10;
		size_t v_top = 0;
		size_t v_position = 0;
		size_t v_line = 0;
		size_t v_target = 0;
		size_t v_column = 0;
		size_t v_x = 0;
		size_t v_y = 0;

		void f_y(const decltype(rows.f_begin())& a_row)
		{
			v_y = a_row.f_index().v_y;
			if (v_y < v_top) {
				v_top = v_y;
			} else {
				size_t bottom = v_y + a_row.f_delta().v_y;
				if (bottom > v_top + v_height) v_top = bottom - v_height;
			}
			size_t n = std::max(v_rows.f_size().v_i, v_height) - v_height;
			if (v_top > n) v_top = n;
		}
		void f_from_line()
		{
			auto line = v_lines.f_at(v_line);
			size_t p = line.f_index().v_i1;
			size_t ax = v_rows.f_at_in_text(p).f_index().v_x + v_target;
			auto row = v_rows.f_at_in_text(p + line.f_delta().v_i1);
			if (ax < row.f_index().v_x)
				row = v_rows.f_at_in_x(ax);
			else
				--row;
			auto x = v_rows.f_each_x(row, [&](size_t p, size_t x, size_t width)
			{
				return x + width <= ax;
			});
			v_position = std::get<0>(x);
			v_column = v_position - line.f_index().v_i1;
			v_x = std::get<1>(x) - row.f_index().v_x;
			f_y(row);
		}
		void f_from_position()
		{
			auto line = v_lines.f_at_in_text(v_position);
			v_line = line.f_index().v_i0;
			v_column = v_position - line.f_index().v_i1;
			auto row = v_rows.f_at_in_text(v_position);
			auto x = v_rows.f_each_x(row, [&](size_t p, size_t x, size_t width)
			{
				return p < v_position;
			});
			size_t ax = std::get<1>(x);
			if (v_target < ax || v_column < line.f_delta().v_i1 - 1 && v_target >= ax + std::get<2>(x)) v_target = ax - v_rows.f_at_in_text(line.f_index().v_i1).f_index().v_x;
			v_x = ax - row.f_index().v_x;
			f_y(row);
		}

		nata::t_slot<size_t, size_t, size_t> v_replaced = [this](auto a_p, auto a_n0, auto a_n1)
		{
			if (v_position < a_p) return;
			if (v_position > a_p || a_n0 <= 0) v_position = (v_position < a_p + a_n0 ? a_p : v_position - a_n0) + a_n1;
			f_from_position();
		};
		nata::t_slot<size_t, size_t> v_painted = [this](auto a_p, auto a_n)
		{
			f_from_position();
		};
		nata::t_slot<> v_resized = [this]
		{
			f_from_position();
		};
	} state{text.f_lines(), rows};
	if (argc > 1) {
		std::wifstream in(argv[1]);
		in.imbue(std::locale(""));
		while (in.good()) {
			wchar_t cs[8];
			in.read(cs, sizeof(cs) / sizeof(wchar_t));
			text.f_replace(text.f_size(), 0, cs, cs + in.gcount());
		}
	}
	rows.v_replaced >> state.v_replaced;
	rows.v_painted >> state.v_painted;
	rows.v_resized >> state.v_resized;
	while (true) {
		render_rows(state.v_top, 0, state.v_height);
		mvprintw(state.v_height, 0, "T:%d L:%d C:%d", state.v_top, state.v_line, state.v_column);
		clrtoeol();
		move(state.v_y - state.v_top, state.v_x);
		refresh();
		wint_t c;
		if (get_wch(&c) != ERR) {
			if (c == 0x1b) break;
			switch (c) {
			case KEY_DOWN:
				if (state.v_line < text.f_lines().f_size().v_i0 - 1) {
					++state.v_line;
					state.f_from_line();
				}
				break;
			case KEY_UP:
				if (state.v_line > 0) {
					--state.v_line;
					state.f_from_line();
				}
				break;
			case KEY_LEFT:
				if (state.v_position > 0) {
					--state.v_position;
					state.f_from_position();
				}
				break;
			case KEY_RIGHT:
				if (state.v_position < text.f_size()) {
					++state.v_position;
					state.f_from_position();
				}
				break;
			case KEY_BACKSPACE:
				if (state.v_position > 0) text.f_replace(state.v_position - 1, 1, &c, &c);
				break;
			case KEY_F(1):
				target.f_width__(10);
				break;
			case KEY_F(2):
				target.f_width__(20);
				break;
			case KEY_F(3):
				target.f_width__(40);
				break;
			case KEY_F(4):
				{
					std::wregex keywords(L"(#.*(?:\\n|$))|(?:\\b(if|then|elif|else|fi|case|esac|for|in|do|done|break|continue|return)\\b)");
					std::regex_iterator<decltype(text.f_begin()), wchar_t> last;
					for (decltype(last) i(text.f_begin(), text.f_end(), keywords); i != last; ++i) {
						auto match = *i;
						tokens.f_paint(match.position(), {{
							match[1].length() > 0 ? attribute_comment : attribute_keyword,
							size_t(match.length())
						}});
					}
				}
				break;
			case KEY_ENTER:
			case L'\r':
				c = L'\n';
			default:
				text.f_replace(state.v_position, 0, &c, &c + 1);
			}
		}
	}
	return 0;
}
