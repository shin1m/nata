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
	constexpr attr_t attribute_control = A_DIM | COLOR_PAIR(1);
	init_pair(2, COLOR_BLUE, -1);
	constexpr attr_t attribute_comment = COLOR_PAIR(2);
	init_pair(3, COLOR_YELLOW, -1);
	constexpr attr_t attribute_keyword = COLOR_PAIR(3);
	nata::t_text<nata::t_lines<5, 5>, 5, 5> text;
	nata::t_tokens<decltype(text), attr_t, 5, 5> tokens(text);
	struct
	{
		nata::t_signal<> v_resized;

		size_t f_width() const
		{
			return COLS;
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
		void f_scroll(size_t a_y, size_t a_height, int a_delta)
		{
			setscrreg(a_y, a_height - 1);
			scrl(a_delta);
			setscrreg(0, LINES - 1);
		}
	} target;
	nata::t_rows<decltype(tokens), decltype(target), 5, 5> rows(tokens, target);
	struct t_graphics
	{
		const decltype(target)& v_target;
		size_t v_to;
		size_t v_x = 0;
		cchar_t v_c{A_NORMAL, L" "};

		void f_move(size_t a_y)
		{
			v_x = 0;
			move(v_to + a_y, 0);
		}
		void f_attribute(attr_t a_a)
		{
			v_c.attr = a_a;
		}
		void f_put(wchar_t a_c)
		{
			v_c.chars[0] = a_c;
			add_wch(&v_c);
			v_x += std::get<0>(v_target.f_size(a_c, v_c.attr));
		}
		void f_tab()
		{
			size_t w = std::get<0>(v_target.f_tab(v_x, v_c.attr));
			if (v_x % 8 == 0) {
				cchar_t cc{attribute_control, L"|"};
				add_wch(&cc);
				++v_x;
				--w;
			}
			v_c.chars[0] = L' ';
			while (w > 0) {
				add_wch(&v_c);
				++v_x;
				--w;
			}
		}
		void f_wrap()
		{
			clrtoeol();
		}
		void f_eol()
		{
			cchar_t cc{attribute_control, L"/"};
			add_wch(&cc);
			clrtoeol();
		}
		void f_eof()
		{
			cchar_t cc{attribute_control, L"<"};
			add_wch(&cc);
			clrtoeol();
		}
		void f_empty(size_t a_y)
		{
			move(v_to + a_y, 0);
			clrtoeol();
		}
	};
	nata::t_stretches<bool, 5, 5> region;
	region.f_replace(0, 0, {{true, 8}});
	struct
	{
		const decltype(text.f_lines())& v_lines;
		const decltype(rows)& v_rows;
		decltype(region)& v_region;
		size_t v_top = 0;
		size_t v_position = 0;
		size_t v_line = 0;
		size_t v_target = 0;
		size_t v_column = 0;
		size_t v_x = 0;
		size_t v_y = 0;

		void f_dirty(size_t a_y, size_t a_height, bool a_dirty)
		{
			v_region.f_replace(a_y, a_height, {{a_dirty, a_height}});
		}
		void f_render(t_graphics& a_target)
		{
			auto row = v_rows.f_at_in_y(v_top);
			auto text = v_rows.v_tokens.v_text.f_at(row.f_index().v_text);
			auto token = v_rows.v_tokens.f_at_in_text(text.f_index());
			size_t delta = token.f_index().v_i1 + token.f_delta().v_i1 - text.f_index();
			auto next = [&](size_t a_d)
			{
				delta -= a_d;
				if (delta <= 0) delta = (++token).f_delta().v_i1;
			};
			auto dirty = v_region.f_begin();
			auto dend = v_region.f_end();
			if (dirty != dend && !dirty->v_x) ++dirty;
			size_t y = 0;
			while (dirty != dend) {
				auto rd = row.f_delta();
				if (row->v_tail) --rd.v_text;
				if (y + rd.v_y > dirty.f_index().v_i1) {
					a_target.f_move(y);
					size_t x = 0;
					while (rd.v_text > 0) {
						a_target.f_attribute(token->v_x);
						size_t td = std::min(delta, rd.v_text);
						for (size_t i = 0; i < td; ++i) {
							wchar_t c = *text;
							if (c == L'\t')
								a_target.f_tab();
							else
								a_target.f_put(c);
							++text;
						}
						next(td);
						rd.v_text -= td;
					}
					y += rd.v_y;
					if (dirty.f_index().v_i1 + dirty.f_delta().v_i1 <= y) {
						do ++dirty; while (dirty != dend && dirty.f_index().v_i1 + dirty.f_delta().v_i1 <= y);
						if (dirty != dend && !dirty->v_x) ++dirty;
					}
					if (row->v_tail) {
						if (++row == v_rows.f_end()) {
							a_target.f_eof();
							break;
						}
						a_target.f_eol();
						++text;
						next(1);
					} else {
						++row;
						a_target.f_wrap();
					}
				} else {
					while (rd.v_text > 0) {
						size_t td = std::min(delta, rd.v_text);
						for (size_t i = 0; i < td; ++i) ++text;
						next(td);
						rd.v_text -= td;
					}
					y += rd.v_y;
					if (row->v_tail) {
						if (++row == v_rows.f_end()) break;
						++text;
						next(1);
					} else {
						++row;
					}
				}
			}
			if (y < dirty.f_index().v_i1) y = dirty.f_index().v_i1;
			while (dirty != dend) {
				for (size_t b = dirty.f_index().v_i1 + dirty.f_delta().v_i1; y < b; ++y) a_target.f_empty(y);
				if (++dirty == dend) break;
				y = (++dirty).f_index().v_i1;
			}
			f_dirty(0, v_region.f_size().v_i1, false);
		}
		void f_scroll(size_t a_y, int a_delta)
		{
			size_t height = v_region.f_size().v_i1;
			size_t range = height - a_y;
			if (a_delta >= range) {
				f_dirty(a_y, range, true);
				return;
			}
			if (a_delta < 0) {
				v_region.f_replace(height + a_delta, -a_delta, {});
				v_region.f_replace(a_y, 0, {{true, size_t(-a_delta)}});
			} else {
				v_region.f_replace(a_y, a_delta, {});
				v_region.f_replace(height - a_delta, 0, {{true, size_t(a_delta)}});
			}
			v_rows.v_target.f_scroll(a_y, height, a_delta);
		}
		void f_top__(size_t a_value)
		{
			size_t h = v_region.f_size().v_i1;
			size_t n = std::max(v_rows.f_size().v_i, h) - h;
			if (a_value > n) a_value = n;
			if (a_value == v_top) return;
			f_scroll(0, int(a_value) - int(v_top));
			v_top = a_value;
		}
		void f_y(const decltype(rows.f_begin())& a_row)
		{
			v_y = a_row.f_index().v_y;
			size_t top = v_top;
			if (v_y < top) {
				top = v_y;
			} else {
				size_t b = v_y + a_row.f_delta().v_y;
				size_t h = v_region.f_size().v_i1;
				if (b > top + h) top = b - h;
			}
			f_top__(top);
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
		void f_from_position(bool a_retarget = false)
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
			if (a_retarget || v_target < ax || v_column < line.f_delta().v_i1 - 1 && v_target >= ax + std::get<2>(x)) v_target = ax - v_rows.f_at_in_text(line.f_index().v_i1).f_index().v_x;
			v_x = ax - row.f_index().v_x;
			f_y(row);
		}

		void f_adjust(size_t a_y, size_t a_h0, size_t a_h1)
		{
			int y = a_y - v_top;
			int height = v_region.f_size().v_i1;
			if (y >= height) return;
			int b0 = y + int(a_h0);
			int b1 = y + int(a_h1);
			if (std::max(b0, b1) >= height) {
				if (y < 0) y = 0;
				f_dirty(y, height - y, true);
			} else if (b0 <= 0) {
				v_top += b1 - b0;
			} else if (b1 <= 0) {
				f_scroll(0, b0);
				v_top = b1;
			} else {
				if (y < 0) y = 0;
				int b = std::min(b0, b1);
				f_dirty(y, b - y, true);
				f_scroll(b, b0 - b1);
			}
			f_top__(v_top);
		}
		nata::t_slot<size_t, size_t, size_t, size_t, size_t, size_t> v_replaced = [this](auto a_p, auto a_n0, auto a_n1, auto a_y, auto a_h0, auto a_h1)
		{
			f_adjust(a_y, a_h0, a_h1);
			if (v_position < a_p) return;
			if (v_position > a_p || a_n0 <= 0) v_position = (v_position < a_p + a_n0 ? a_p : v_position - a_n0) + a_n1;
			f_from_position();
		};
		nata::t_slot<size_t, size_t, size_t, size_t, size_t> v_painted = [this](auto a_p, auto a_n, auto a_y, auto a_h0, auto a_h1)
		{
			f_adjust(a_y, a_h0, a_h1);
			f_from_position();
		};
	} state{text.f_lines(), rows, region};
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
	while (true) {
		{
			t_graphics g{target, 0};
			state.f_render(g);
		}
		mvprintw(region.f_size().v_i1, 0, "T:%d L:%d C:%d", state.v_top, state.v_line, state.v_column);
		clrtoeol();
		move(state.v_y - state.v_top, state.v_x);
		refresh();
		wint_t c;
		if (get_wch(&c) != ERR) {
			if (c == 0x1b) break;
			switch (c) {
			case KEY_RESIZE:
				target.v_resized();
				break;
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
					state.f_from_position(true);
				}
				break;
			case KEY_RIGHT:
				if (state.v_position < text.f_size()) {
					++state.v_position;
					state.f_from_position(true);
				}
				break;
			case KEY_BACKSPACE:
				if (state.v_position > 0) text.f_replace(state.v_position - 1, 1, &c, &c);
				break;
			case KEY_F(1):
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
