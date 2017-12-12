#include "view.h"
#include "widget.h"
#include "curses.h"
#include <fstream>
#include <regex>

#if 0
constexpr size_t c_text_chunk = 4096;
constexpr size_t c_lines_chunk = 256;
constexpr size_t c_tokens_chunk = 256;
constexpr size_t c_foldings_chunk = 256;
constexpr size_t c_rows_chunk = 256;
constexpr size_t c_tokens_paint_unit = 256;
#else
constexpr size_t c_text_chunk = 5;
constexpr size_t c_lines_chunk = 5;
constexpr size_t c_tokens_chunk = 5;
constexpr size_t c_foldings_chunk = 5;
constexpr size_t c_rows_chunk = 5;
constexpr size_t c_tokens_paint_unit = 4;
#endif

int main(int argc, char* argv[])
{
	nata::curses::t_session session;
	init_pair(1, COLOR_WHITE, -1);
	constexpr attr_t attribute_control = A_DIM | COLOR_PAIR(1);
	init_pair(2, COLOR_BLACK, COLOR_WHITE);
	constexpr attr_t attribute_folded = A_DIM | COLOR_PAIR(2);
	init_pair(3, COLOR_BLUE, -1);
	constexpr attr_t attribute_comment = COLOR_PAIR(3);
	init_pair(4, COLOR_YELLOW, -1);
	constexpr attr_t attribute_keyword = COLOR_PAIR(4);
	nata::t_text<nata::t_lines<c_lines_chunk, c_lines_chunk>, c_text_chunk, c_text_chunk> text;
	nata::t_tokens<decltype(text), attr_t, c_tokens_chunk, c_tokens_chunk> tokens(text);
	nata::curses::t_target target;
	nata::t_rows<decltype(tokens), nata::t_nested<nata::t_foldable, c_foldings_chunk, c_foldings_chunk>, decltype(target), c_rows_chunk, c_rows_chunk> rows(tokens, target);
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
		struct t_block
		{
			size_t v_type;
			std::deque<decltype(rows)::t_foldings::t_span> v_xs;
		};

		decltype(rows)& v_rows;
		std::wregex v_pattern{L"(#.*(?:\\n|$))|(?:\\b(if|(else)|(then)|(case)|(do)|(elif|fi)|(esac)|(done)|for|in|break|continue|return)\\b)", std::regex_constants::ECMAScript | std::regex_constants::optimize};
		std::regex_iterator<decltype(text.f_begin()), wchar_t> v_eos;
		std::regex_iterator<decltype(text.f_begin()), wchar_t> v_i;
		size_t v_p;
		std::wstring v_message;
		std::deque<t_block> v_nesting;
		size_t v_nesting_p;
		size_t v_nesting_n;

		operator bool() const
		{
			return v_p < v_rows.v_tokens.v_text.f_size();
		}
		void operator()()
		{
			if (!*this) return;
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
			auto plain = [&]
			{
				if (v_nesting_n <= 0) return;
				v_nesting.back().v_xs.emplace_back(v_nesting_n);
				v_nesting_n = 0;
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
				auto open = [&](size_t type)
				{
					plain();
					v_nesting.push_back({type});
				};
				auto close = [&]
				{
					v_nesting_n += m0.first.f_index() - v_p;
					plain();
					auto xs = std::move(v_nesting.back().v_xs);
					if (xs.empty()) {
						v_nesting.pop_back();
					} else {
						size_t n = xs.front().v_n;
						xs.pop_front();
						v_nesting.pop_back();
						v_nesting.back().v_xs.emplace_back(n, std::move(xs));
					}
					if (v_nesting.back().v_type == 0) {
						v_rows.f_foldable(v_nesting_p, std::move(v_nesting.back().v_xs));
						v_nesting_p = m0.first.f_index();
					}
					v_nesting_n += m0.second.f_index() - m0.first.f_index();
				};
				size_t type = 3;
				for (; type < v_i->size(); ++type) {
					auto& m = (*v_i)[type];
					if (m.first != m.second) break;
				}
				switch (type) {
				case 3:
					if (v_nesting.back().v_type == 4) {
						close();
						open(4);
					} else {
						v_nesting_n += m0.second.f_index() - v_p;
					}
					break;
				case 4:
				case 5:
				case 6:
					v_nesting_n += m0.second.f_index() - v_p;
					open(type);
					break;
				case 7:
				case 8:
				case 9:
					if (v_nesting.back().v_type == type - 3)
						close();
					else
						v_nesting_n += m0.second.f_index() - v_p;
					break;
				default:
					v_nesting_n += m0.second.f_index() - v_p;
				}
				v_p = m0.second.f_index();
				++v_i;
			}
			v_rows.v_tokens.f_paint(p, std::move(xs));
			size_t n = v_rows.v_tokens.v_text.f_size();
			if (v_i != v_eos) {
				std::wostringstream s;
				s << L"running: " << v_p * 100 / n << L'%';
				v_message = s.str();
			} else {
				v_nesting_n += n - v_p;
				plain();
				while (v_nesting.size() > 1) {
					auto xs = std::move(v_nesting.back().v_xs);
					v_nesting.pop_back();
					if (!xs.empty() && !xs.front().v_x && !v_nesting.back().v_xs.empty() && !v_nesting.back().v_xs.back().v_x) {
						v_nesting.back().v_xs.back().v_n += xs.front().v_n;
						xs.pop_front();
					}
					v_nesting.back().v_xs.insert(v_nesting.back().v_xs.end(), xs.begin(), xs.end());
				}
				size_t m = std::accumulate(v_nesting.back().v_xs.begin(), v_nesting.back().v_xs.end(), 0, [](size_t n, const auto& x)
				{
					return n + x.v_n;
				});
				v_rows.f_foldable(v_nesting_p, std::move(v_nesting.back().v_xs));
				v_nesting.clear();
				if (v_p < n) {
					v_rows.v_tokens.f_paint(v_p, {{0, n - v_p}});
					v_p = n;
				}
				v_message.clear();
			}
		}

		nata::t_slot<size_t, size_t, size_t> v_replaced = [this](auto, auto, auto)
		{
			v_i = decltype(v_i)(v_rows.v_tokens.v_text.f_begin(), v_rows.v_tokens.v_text.f_end(), v_pattern);
			v_p = v_nesting_p = v_nesting_n = 0;
			v_nesting.clear();
			v_nesting.push_back({0});
		};
	} task{rows};
	text.v_replaced >> task.v_replaced;
	task.v_replaced(0, 0, 0);
	while (true) {
		{
			nata::curses::t_graphics g{target, attribute_control, attribute_folded, 0};
			widget.f_render(g);
		}
		size_t& position = std::get<0>(widget.v_position);
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
		move(widget.v_row.f_index().v_y - widget.v_top, std::get<1>(widget.v_position) - widget.v_row.f_index().v_x);
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
				if (position > 0) {
					std::vector<decltype(rows)::t_foldings::t_iterator> folding;
					size_t p = rows.f_leaf_at_in_text(--position, folding);
					if (folding.back()->v_x) position -= p;
					widget.f_from_position(true);
				}
				break;
			case KEY_RIGHT:
				if (position < text.f_size()) {
					std::vector<decltype(rows)::t_foldings::t_iterator> folding;
					size_t p = rows.f_leaf_at_in_text(++position, folding);
					if (p > 0 && folding.back()->v_x) position += folding.back().f_delta().v_i1 - p;
					widget.f_from_position(true);
				}
				break;
			case KEY_BACKSPACE:
				if (position > 0) text.f_replace(position - 1, 1, &c, &c);
				break;
			case KEY_F(1):
				rows.f_folded(position, true);
				break;
			case KEY_F(2):
				rows.f_folded(position, false);
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
