#include "widget.h"
#include "painter.h"
#include "folder.h"
#include "curses.h"
#include <fstream>
#include <regex>

#if 0
constexpr size_t c_text_chunk = 4096;
constexpr size_t c_lines_chunk = 256;
constexpr size_t c_tokens_chunk = 256;
constexpr size_t c_foldings_chunk = 16;
constexpr size_t c_rows_chunk = 256;
constexpr size_t c_region_chunk = 8;
constexpr size_t c_task_unit = 256;
#else
constexpr size_t c_text_chunk = 5;
constexpr size_t c_lines_chunk = 5;
constexpr size_t c_tokens_chunk = 5;
constexpr size_t c_foldings_chunk = 5;
constexpr size_t c_rows_chunk = 5;
constexpr size_t c_region_chunk = 5;
constexpr size_t c_task_unit = 4;
#endif

int main(int argc, char* argv[])
{
	nata::curses::t_session session;
	init_pair(1, COLOR_WHITE, -1);
	constexpr attr_t attribute_control = A_DIM | COLOR_PAIR(1);
	init_pair(2, COLOR_BLACK, COLOR_WHITE);
	constexpr attr_t attribute_folded = A_DIM | COLOR_PAIR(2);
	constexpr attr_t attribute_selected = A_REVERSE;
	init_pair(4, -1, COLOR_YELLOW);
	constexpr attr_t attribute_highlighted = COLOR_PAIR(4);
	init_pair(5, COLOR_BLUE, -1);
	constexpr attr_t attribute_comment = COLOR_PAIR(5);
	init_pair(6, COLOR_YELLOW, -1);
	constexpr attr_t attribute_keyword = COLOR_PAIR(6);
	nata::t_text<nata::t_lines<c_lines_chunk, c_lines_chunk>, c_text_chunk, c_text_chunk> text;
	if (argc > 1) {
		std::wifstream in(argv[1]);
		in.imbue(std::locale(""));
		while (in.good()) {
			wchar_t cs[8];
			in.read(cs, sizeof(cs) / sizeof(wchar_t));
			text.f_replace(text.f_size(), 0, cs, cs + in.gcount());
		}
	}
	nata::t_tokens<decltype(text), attr_t, c_tokens_chunk, c_tokens_chunk> tokens(text);
	nata::curses::t_target target;
	nata::t_rows<decltype(tokens), decltype(target), nata::t_foldable<c_foldings_chunk, c_foldings_chunk>, c_rows_chunk, c_rows_chunk> rows(tokens, target);
/*	nata::t_slot<size_t, size_t, size_t, size_t, size_t> rows_painted = [](auto a_p, auto a_n, auto a_y, auto a_h0, auto a_h1)
	{
		std::fprintf(stderr, "painted: (%d, %d), (%d, %d -> %d)\n", a_p, a_n, a_y, a_h0, a_h1);
	};
	rows.v_painted >> rows_painted;*/
	nata::t_widget<decltype(rows), nata::t_stretches<bool, c_region_chunk, c_region_chunk>> widget(rows, LINES - 1);
	widget.f_add_overlay(attribute_highlighted);
	widget.f_add_overlay(attribute_selected);
	nata::t_painter<decltype(tokens)> painter(tokens);
	nata::t_folder<decltype(rows), size_t> folder(rows);
	struct
	{
		decltype(rows)& v_rows;
		decltype(painter)& v_painter;
		decltype(folder)& v_folder;
		std::wregex v_pattern{L"(#.*(?:\\n|$))|(?:\\b(?:(if|for|in|break|continue|return)|(else)|(then)|(case)|(do)|(elif|fi)|(esac)|(done))\\b)", std::regex_constants::ECMAScript | std::regex_constants::optimize};
		std::regex_iterator<decltype(text.f_begin()), wchar_t> v_eos;
		std::regex_iterator<decltype(text.f_begin()), wchar_t> v_i;
		std::wstring v_message;

		operator bool() const
		{
			return v_painter.f_p() < v_rows.v_tokens.v_text.f_size();
		}
		void operator()()
		{
			if (!*this) return;
			for (size_t i = 0; i < c_task_unit; ++i) {
				if (v_i == v_eos) break;
				size_t type = 1;
				for (; type < v_i->size(); ++type) {
					auto& m = (*v_i)[type];
					if (m.first != m.second) break;
				}
				auto& m = (*v_i)[0];
				size_t a = m.first.f_index() - v_painter.f_p();
				size_t b = m.second.f_index() - m.first.f_index();
				v_painter.f_push(0, a, 64);
				v_painter.f_push(type == 1 ? attribute_comment : attribute_keyword, b, 64);
				auto close = [&]
				{
					v_folder.f_push(a);
					v_folder.f_close();
					v_folder.f_push(b);
				};
				switch (type) {
				case 3:
					if (v_folder.f_tag() == 4) {
						close();
						v_folder.f_open(4);
					} else {
						v_folder.f_push(a + b);
					}
					break;
				case 4:
				case 5:
				case 6:
					v_folder.f_push(a + b);
					v_folder.f_open(type);
					break;
				case 7:
				case 8:
				case 9:
					if (v_folder.f_tag() == type - 3)
						close();
					else
						v_folder.f_push(a + b);
					break;
				default:
					v_folder.f_push(a + b);
				}
				++v_i;
			}
			size_t n = v_rows.v_tokens.v_text.f_size();
			if (v_i == v_eos) {
				n -= v_painter.f_p();
				v_painter.f_push(0, n, 64);
				v_painter.f_flush();
				v_folder.f_push(n);
				v_folder.f_flush();
				v_message.clear();
			} else {
				v_painter.f_flush();
				std::wostringstream s;
				s << L"running: " << v_painter.f_p() * 100 / n << L'%';
				v_message = s.str();
			}
		}

		nata::t_slot<size_t, size_t, size_t> v_replaced = [this](auto, auto, auto)
		{
			v_i = decltype(v_i)(v_rows.v_tokens.v_text.f_begin(), v_rows.v_tokens.v_text.f_end(), v_pattern);
			v_painter.f_reset();
			v_folder.f_reset();
		};
	} task{rows, painter, folder};
	text.v_replaced >> task.v_replaced;
	task.v_replaced(0, 0, 0);
	wchar_t prefix = L'\0';
	while (true) {
		{
			nata::curses::t_graphics g{target, attribute_control, attribute_folded, 0, L'0' + prefix};
			widget.f_render(g);
			prefix = (prefix + 1) % 10;
		}
		size_t position = std::get<0>(widget.v_position);
		if (task.v_message.empty()) {
			auto line = text.f_lines().f_at_in_text(position).f_index();
			size_t column = position - line.v_i1;
			size_t x = std::get<1>(widget.v_position) - widget.v_line.v_x;
			size_t n = widget.f_range();
			mvprintw(widget.f_height(), 0, "%d,%d-%d %d%%", line.v_i0, column, x, n > 0 ? widget.v_top * 100 / n : 100);
		} else {
			mvaddwstr(widget.f_height(), 0, task.v_message.c_str());
		}
		clrtobot();
		target.f_move(std::get<1>(widget.v_position) - widget.v_row.f_index().v_x, widget.v_row.f_index().v_y - widget.v_top);
		refresh();
		timeout(task ? 0 : -1);
		wint_t c;
		if (get_wch(&c) != ERR) {
			if (c == 0x1b) break;
			switch (c) {
			case KEY_RESIZE:
				widget.f_height__(LINES - 1);
				target.v_resized();
				break;
			case KEY_DOWN:
				if (widget.v_line.v_line < rows.f_size().v_line - 1) {
					++widget.v_line.v_line;
					widget.f_from_line();
				}
				break;
			case KEY_UP:
				if (widget.v_line.v_line > 0) {
					--widget.v_line.v_line;
					widget.f_from_line();
				}
				break;
			case KEY_LEFT:
				if (position > 0) widget.f_position__(position - 1, false);
				break;
			case KEY_RIGHT:
				if (position < text.f_size()) widget.f_position__(position + 1, true);
				break;
			case KEY_BACKSPACE:
				if (position > 0) {
					rows.f_folded(--position, false);
					text.f_replace(position, 1, &c, &c);
				}
				break;
			case KEY_F(1):
				widget.f_position__(rows.f_folded(position, true), false);
				break;
			case KEY_F(2):
				rows.f_folded(position, false);
				break;
			case KEY_F(3):
				if (position < text.f_size()) {
					widget.f_overlay(0, position, 1, true);
					widget.f_position__(position + 1, true);
				}
				break;
			case KEY_F(4):
				widget.f_overlay(0, 0, text.f_size(), false);
				break;
			case KEY_F(5):
				if (position < text.f_size()) {
					widget.f_overlay(1, position, 1, true);
					widget.f_position__(position + 1, true);
				}
				break;
			case KEY_F(6):
				widget.f_overlay(1, 0, text.f_size(), false);
				break;
			case KEY_ENTER:
			case L'\r':
				c = L'\n';
			default:
				text.f_replace(position, 0, &c, &c + 1);
			}
			widget.f_into_view(widget.v_row);
		}
		task();
	}
	return 0;
}
