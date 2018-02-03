#include "model.h"
#include "rows.h"
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
	init_pair(2, COLOR_BLACK, COLOR_WHITE);
	constexpr attr_t attribute_selected = A_REVERSE;
	init_pair(3, -1, COLOR_YELLOW);
	constexpr attr_t attribute_highlighted = COLOR_PAIR(3);
	init_pair(4, COLOR_BLUE, -1);
	constexpr attr_t attribute_comment = COLOR_PAIR(4);
	init_pair(5, COLOR_YELLOW, -1);
	constexpr attr_t attribute_keyword = COLOR_PAIR(5);
	nata::t_text<nata::t_lines<c_lines_chunk, c_lines_chunk>, c_text_chunk, c_text_chunk> text;
	if (argc > 1) {
		std::wifstream in(argv[1]);
		in.imbue(std::locale(""));
		while (in.good()) {
			wchar_t cs[256];
			in.read(cs, sizeof(cs) / sizeof(wchar_t));
			text.f_replace(text.f_size(), 0, cs, cs + in.gcount());
		}
	}
	nata::t_tokens<decltype(text), attr_t, c_tokens_chunk, c_tokens_chunk> tokens(text);
	nata::curses::t_target target{A_DIM | COLOR_PAIR(1), A_DIM | COLOR_PAIR(2)};
	nata::t_rows<decltype(tokens), decltype(target), nata::t_foldable<c_foldings_chunk, c_foldings_chunk>, c_rows_chunk, c_rows_chunk> rows(tokens, target);
/*	nata::t_slot<size_t, size_t, size_t, size_t, size_t> rows_painted = [](auto a_p, auto a_n, auto a_y, auto a_h0, auto a_h1)
	{
		std::fprintf(stderr, "painted: (%d, %d), (%d, %d -> %d)\n", a_p, a_n, a_y, a_h0, a_h1);
	};
	rows.v_painted >> rows_painted;*/
	nata::t_widget<decltype(rows), nata::t_stretches<bool, c_region_chunk, c_region_chunk>> widget(rows, LINES - 1);
	widget.f_add_overlay(attribute_highlighted);
	widget.f_add_overlay(attribute_selected);
	struct
	{
		decltype(rows)& v_rows;
		nata::t_painter<decltype(tokens)> v_painter{v_rows.v_tokens};
		nata::t_folder<decltype(rows), size_t> v_folder{v_rows};
		std::wregex v_pattern{L"(#.*(?:\\n|$))|(?:\\b(?:(if|for|in|break|continue|return)|(else)|(then)|(case)|(do)|(elif|fi)|(esac)|(done))\\b)", std::wregex::ECMAScript | std::wregex::optimize};
		std::regex_iterator<decltype(text.f_begin()), wchar_t> v_eos;
		std::regex_iterator<decltype(text.f_begin()), wchar_t> v_i;
		std::wstring v_message;

		operator bool() const
		{
			return v_painter.f_current() < v_rows.v_tokens.v_text.f_size();
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
				size_t a = m.first.f_index() - v_painter.f_current();
				size_t b = m.second.f_index() - m.first.f_index();
				v_painter.f_push(A_NORMAL, a, 64);
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
				n -= v_painter.f_current();
				v_painter.f_push(A_NORMAL, n, 64);
				v_painter.f_flush();
				v_folder.f_push(n);
				v_folder.f_flush();
				v_message.clear();
			} else {
				v_painter.f_flush();
				std::wostringstream s;
				s << L"running: " << v_painter.f_current() * 100 / n << L'%';
				v_message = s.str();
			}
		}

		nata::t_slot<size_t, size_t, size_t> v_replaced = [this](auto, auto, auto)
		{
			v_i = decltype(v_i)(v_rows.v_tokens.v_text.f_begin(), v_rows.v_tokens.v_text.f_end(), v_pattern);
			v_painter.f_reset();
			v_folder.f_reset();
		};
	} syntax{rows};
	text.v_replaced >> syntax.v_replaced;
	syntax.v_replaced(0, 0, 0);
	struct
	{
		decltype(*widget.f_overlays()[0].second) v_overlay;
		nata::t_painter<std::decay_t<decltype(v_overlay)>> v_painter{v_overlay};
		std::wregex v_pattern;
		std::regex_iterator<decltype(text.f_begin()), wchar_t> v_eos;
		std::regex_iterator<decltype(text.f_begin()), wchar_t> v_i;

		operator bool() const
		{
			return v_painter.f_current() < v_overlay.v_text.f_size();
		}
		void operator()()
		{
			if (!*this) return;
			for (size_t i = 0; i < c_task_unit; ++i) {
				if (v_i == v_eos) break;
				auto& m = (*v_i)[0];
				v_painter.f_push(false, m.first.f_index() - v_painter.f_current(), 64);
				v_painter.f_push(true, m.second.f_index() - m.first.f_index(), 64);
				++v_i;
			}
			if (v_i == v_eos) v_painter.f_push(false, v_overlay.v_text.f_size() - v_painter.f_current(), 64);
			v_painter.f_flush();
		}

		nata::t_slot<size_t, size_t, size_t> v_replaced = [this](auto, auto, auto)
		{
			v_i = decltype(v_i)(v_overlay.v_text.f_begin(), v_overlay.v_text.f_end(), v_pattern);
			v_painter.f_reset();
		};
	} search{*widget.f_overlays()[0].second};
	text.v_replaced >> search.v_replaced;
	search.v_replaced(0, 0, 0);
	while (true) {
		{
			nata::curses::t_graphics g{target, 0};
			widget.f_render(g);
		}
		widget.f_status(syntax.v_message);
		target.f_move(std::get<1>(widget.v_position) - widget.v_row.f_index().v_x, widget.v_row.f_index().v_y - widget.v_top);
		timeout(syntax || search ? 0 : -1);
		wint_t c;
		if (get_wch(&c) != ERR) {
			if (c == 0x1b) break;
			auto clear_overlay = [&](auto& overlay)
			{
				nata::t_painter<std::decay_t<decltype(overlay)>> painter(overlay);
				painter.f_reset();
				painter.f_push(false, text.f_size());
				painter.f_flush();
			};
			size_t position = std::get<0>(widget.v_position);
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
				{
					auto& overlay = *widget.f_overlays()[1].second;
					std::wstring s;
					for (auto i = overlay.f_begin(); i != overlay.f_end(); ++i) {
						if (!i->v_x) continue;
						size_t p = i.f_index().v_i1;
						s += {text.f_at(p), text.f_at(p + i.f_delta().v_i1)};
					}
					try {
						search.v_pattern = s;
					} catch (std::regex_error& e) {
						search.v_pattern = {};
					}
					clear_overlay(overlay);
					search.v_replaced(0, 0, 0);
				}
				break;
			case KEY_F(4):
				search.v_pattern = {};
				search.v_replaced(0, 0, 0);
				break;
			case KEY_F(5):
				if (position < text.f_size()) {
					widget.f_overlays()[1].second->f_paint(position, {{true, 1}});
					widget.f_position__(position + 1, true);
				}
				break;
			case KEY_F(6):
				clear_overlay(*widget.f_overlays()[1].second);
				break;
			case KEY_ENTER:
			case L'\r':
				c = L'\n';
			default:
				text.f_replace(position, 0, &c, &c + 1);
			}
			widget.f_into_view(widget.v_row);
		}
		syntax();
		search();
	}
	return 0;
}
