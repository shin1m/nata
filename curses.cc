#include "view.h"
#include "widget.h"
#include "curses.h"
#include <fstream>
#include <regex>

#if 0
constexpr size_t c_text_chunk = 4096;
constexpr size_t c_lines_chunk = 256;
constexpr size_t c_tokens_chunk = 256;
constexpr size_t c_rows_chunk = 256;
constexpr size_t c_tokens_paint_unit = 256;
#else
constexpr size_t c_text_chunk = 5;
constexpr size_t c_lines_chunk = 5;
constexpr size_t c_tokens_chunk = 5;
constexpr size_t c_rows_chunk = 5;
constexpr size_t c_tokens_paint_unit = 4;
#endif

int main(int argc, char* argv[])
{
	nata::curses::t_session session;
	init_pair(1, COLOR_WHITE, -1);
	constexpr attr_t attribute_control = A_DIM | COLOR_PAIR(1);
	init_pair(2, COLOR_BLUE, -1);
	constexpr attr_t attribute_comment = COLOR_PAIR(2);
	init_pair(3, COLOR_YELLOW, -1);
	constexpr attr_t attribute_keyword = COLOR_PAIR(3);
	nata::t_text<nata::t_lines<c_lines_chunk, c_lines_chunk>, c_text_chunk, c_text_chunk> text;
	nata::t_tokens<decltype(text), attr_t, c_tokens_chunk, c_tokens_chunk> tokens(text);
	nata::curses::t_target target;
	nata::t_rows<decltype(tokens), decltype(target), c_rows_chunk, c_rows_chunk> rows(tokens, target);
	if (argc > 1) {
		std::wifstream in(argv[1]);
		in.imbue(std::locale(""));
		while (in.good()) {
			wchar_t cs[8];
			in.read(cs, sizeof(cs) / sizeof(wchar_t));
			text.f_replace(text.f_size(), 0, cs, cs + in.gcount());
		}
	}
	nata::t_widget<decltype(rows)> widget(rows, 16);
	struct
	{
		decltype(tokens)& v_tokens;
		std::wregex v_pattern{L"(#.*(?:\\n|$))|(?:\\b(if|then|elif|else|fi|case|esac|for|in|do|done|break|continue|return)\\b)", std::regex_constants::ECMAScript | std::regex_constants::optimize};
		std::regex_iterator<decltype(text.f_begin()), wchar_t> v_eos;
		std::regex_iterator<decltype(text.f_begin()), wchar_t> v_i;
		size_t v_p;
		std::wstring v_message;

		operator bool() const
		{
			return v_p < v_tokens.v_text.f_size();
		}
		void operator()()
		{
			std::deque<decltype(tokens)::t_span> xs;
			auto push = [&](attr_t a, size_t n)
			{
				if (n <= 0) return;
				if (!xs.empty()) {
					auto& x = xs.back();
					if (a == x.v_x) {
						x.v_n += n;
						return;
					}
				}
				xs.push_back({a, n});
			};
			size_t p = v_p;
			for (size_t i = 0; i < c_tokens_paint_unit; ++i) {
				if (v_i == v_eos) break;
				auto& m0 = (*v_i)[0];
				push(0, m0.first.f_index() - v_p);
				auto& m1 = (*v_i)[1];
				push(
					m1.first != m1.second ? attribute_comment : attribute_keyword,
					m0.second.f_index() - m0.first.f_index()
				);
				v_p = m0.second.f_index();
				++v_i;
			}
			v_tokens.f_paint(p, std::move(xs));
			size_t n = v_tokens.v_text.f_size();
			if (v_i != v_eos) {
				std::wostringstream s;
				s << L"running: " << v_p * 100 / n << L'%';
				v_message = s.str();
			} else if (v_p < n) {
				v_tokens.f_paint(v_p, {{0, n - v_p}});
				v_p = n;
				v_message.clear();
			}
		}

		nata::t_slot<size_t, size_t, size_t> v_replaced = [this](auto, auto, auto)
		{
			v_i = decltype(v_i)(v_tokens.v_text.f_begin(), v_tokens.v_text.f_end(), v_pattern);
			v_p = 0;
		};
	} task{tokens};
	tokens.v_text.v_replaced >> task.v_replaced;
	task.v_replaced(0, 0, 0);
	while (true) {
		{
			nata::curses::t_graphics g{target, attribute_control, 0};
			widget.f_render(g);
		}
		if (task.v_message.empty()) {
			size_t n = widget.f_range();
			mvprintw(widget.f_height(), 0, "%d,%d-%d %d%%", widget.v_line, widget.v_column, widget.f_x_in_line(), n > 0 ? widget.v_top * 100 / n : 100);
		} else {
			mvaddwstr(widget.f_height(), 0, task.v_message.c_str());
		}
		clrtobot();
		move(widget.v_y - widget.v_top, widget.v_x);
		refresh();
		timeout(task ? 0 : -1);
		wint_t c;
		if (get_wch(&c) != ERR) {
			if (c == 0x1b) break;
			switch (c) {
			case KEY_RESIZE:
				target.v_resized();
				break;
			case KEY_DOWN:
				if (widget.v_line < text.f_lines().f_size().v_i0 - 1) {
					++widget.v_line;
					widget.f_from_line();
				}
				break;
			case KEY_UP:
				if (widget.v_line > 0) {
					--widget.v_line;
					widget.f_from_line();
				}
				break;
			case KEY_LEFT:
				if (widget.v_position > 0) {
					--widget.v_position;
					widget.f_from_position(true);
				}
				break;
			case KEY_RIGHT:
				if (widget.v_position < text.f_size()) {
					++widget.v_position;
					widget.f_from_position(true);
				}
				break;
			case KEY_BACKSPACE:
				if (widget.v_position > 0) text.f_replace(widget.v_position - 1, 1, &c, &c);
				break;
			case KEY_ENTER:
			case L'\r':
				c = L'\n';
			default:
				text.f_replace(widget.v_position, 0, &c, &c + 1);
			}
			widget.f_into_view(rows.f_at_in_text(widget.v_position));
		}
		task();
	}
	return 0;
}
