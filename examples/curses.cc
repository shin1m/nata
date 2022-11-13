#include <nata/model.h>
#include <nata/rows.h>
#include <nata/widget.h>
#include <nata/painter.h>
#include <nata/creaser.h>
#include <nata/curses.h>
#include <fstream>
#include <regex>
#include "tree_sitter.h"

extern "C" TSLanguage* tree_sitter_json();

#if 0
constexpr size_t c_text_chunk = 4096;
constexpr size_t c_lines_chunk = 256;
constexpr size_t c_tokens_chunk = 256;
constexpr size_t c_creases_chunk = 16;
constexpr size_t c_rows_chunk = 256;
constexpr size_t c_region_chunk = 8;
constexpr size_t c_task_unit = 256;
#else
constexpr size_t c_text_chunk = 5;
constexpr size_t c_lines_chunk = 5;
constexpr size_t c_tokens_chunk = 5;
constexpr size_t c_creases_chunk = 5;
constexpr size_t c_rows_chunk = 5;
constexpr size_t c_region_chunk = 5;
constexpr size_t c_task_unit = 4;
#endif

using t_text = nata::t_text<nata::t_lines<c_lines_chunk, c_lines_chunk>, c_text_chunk, c_text_chunk>;

struct t_view
{
	t_text& v_text;
	nata::t_tokens<t_text, attr_t, c_tokens_chunk, c_tokens_chunk> v_tokens{v_text};
	nata::curses::t_target v_target;
	nata::t_rows<decltype(v_tokens), decltype(v_target), nata::t_creased<c_creases_chunk, c_creases_chunk>, c_rows_chunk, c_rows_chunk> v_rows{v_tokens, v_target};
	nata::t_widget<decltype(v_rows), nata::t_stretches<bool, c_region_chunk, c_region_chunk>> v_widget{v_rows};

	t_view(t_text& a_text, size_t a_x, size_t a_y, size_t a_width, size_t a_height) : v_text(a_text), v_target(a_x, a_y, a_width, a_height)
	{
	}
	void f_render()
	{
		nata::curses::t_graphics g(v_target);
		v_widget.f_render(g);
	}
};

constexpr attr_t attribute_selected = A_REVERSE;
constexpr attr_t attribute_highlighted = COLOR_PAIR(3);
constexpr attr_t attribute_comment = COLOR_PAIR(4);
constexpr attr_t attribute_keyword = COLOR_PAIR(5);

int main(int argc, char* argv[])
{
	using namespace std::literals;
	nata::curses::t_session session;
	init_pair(1, COLOR_WHITE, -1);
	init_pair(2, COLOR_BLACK, COLOR_WHITE);
	init_pair(3, -1, COLOR_YELLOW);
	init_pair(4, COLOR_RED, -1);
	init_pair(5, COLOR_GREEN, -1);
	init_pair(6, COLOR_YELLOW, -1);
	init_pair(7, COLOR_BLUE, -1);
	init_pair(8, COLOR_MAGENTA, -1);
	init_pair(9, COLOR_CYAN, -1);
	t_text text;
	if (argc > 1) {
		std::wifstream in(argv[1]);
		in.imbue(std::locale(""));
		while (in.good()) {
			wchar_t cs[256];
			in.read(cs, sizeof(cs) / sizeof(wchar_t));
			text.f_replace(text.f_size(), 0, cs, cs + in.gcount());
		}
	}
	t_view view(text, 0, 0, COLS, LINES - 1);
	view.v_target.f_attributes(A_DIM | COLOR_PAIR(1), A_DIM | COLOR_PAIR(2));
/*	nata::t_slot<size_t, size_t, size_t, size_t, size_t> rows_painted = [](auto a_p, auto a_n, auto a_y, auto a_h0, auto a_h1)
	{
		std::fprintf(stderr, "painted: (%d, %d), (%d, %d -> %d)\n", a_p, a_n, a_y, a_h0, a_h1);
	};
	view.v_rows.v_painted >> rows_painted;*/
	auto& widget = view.v_widget;
	widget.f_add_overlay(attribute_highlighted);
	widget.f_add_overlay(attribute_selected);
	t_text status;
	t_view strip(status, 0, LINES - 1, COLS, 1);
	struct
	{
		decltype(view.v_rows)& v_rows;
		nata::t_painter<decltype(view.v_tokens)> v_painter{v_rows.v_tokens};
		nata::t_creaser<decltype(view.v_rows)> v_creaser{v_rows};
		nata::tree_sitter::t_query v_query{tree_sitter_json(), R"scm(
(number) @number
(string) @string
(escape_sequence) @escape
[(null) (true) (false)] @literal
(pair key: (_) @key)
["{" "}" "[" "]"] @bracket
(comment) @comment
(object) @object
(array) @array
[(object) (array)] @crease
)scm"sv};
		nata::tree_sitter::t_parser<decltype(text)> v_parser{v_rows.v_tokens.v_text, v_query};
		std::vector<std::pair<attr_t, uint32_t>> v_tokens;
		std::map<std::string_view, attr_t> v_capture2token{
			{"string"sv, COLOR_PAIR(4)},
			{"number"sv, COLOR_PAIR(4)},
			{"literal"sv, COLOR_PAIR(4)},
			{"key"sv, COLOR_PAIR(6)},
			{"keyword"sv, COLOR_PAIR(6)},
			{"escape"sv, COLOR_PAIR(8)},
			{"comment"sv, COLOR_PAIR(7)},
			{"bracket"sv, COLOR_PAIR(8)},
			{"object"sv, COLOR_PAIR(5)},
			{"array"sv, COLOR_PAIR(9)}
		};
		std::vector<std::function<void(uint32_t, uint32_t)>> v_captures;
		std::wstring v_message;

		void f_paint(uint32_t a_p)
		{
			while (true) {
				auto [token, to] = v_tokens.back();
				auto p = a_p < to ? a_p : to;
				auto q = v_painter.f_current();
				if (q < p) v_painter.f_push(token, p - q, 64);
				if (p < to) break;
				v_tokens.pop_back();
			}
		}
		void f_initialize()
		{
			v_rows.v_tokens.v_text.v_replaced >> v_replaced;
			auto count = ts_query_capture_count(v_query);
			for (size_t i = 0; i < count; ++i) {
				uint32_t n;
				auto p = ts_query_capture_name_for_id(v_query, i, &n);
				std::string_view name(p, n);
				if (name == "crease"sv) {
					v_captures.emplace_back([&](uint32_t p, uint32_t n)
					{
						v_creaser.f_push(p - v_creaser.f_current(), n);
					});
				} else {
					auto i = v_capture2token.find(name);
					v_captures.emplace_back([&, token = i == v_capture2token.end() ? A_NORMAL : i->second](uint32_t p, uint32_t n)
					{
						f_paint(p);
						v_tokens.emplace_back(token, p + n);
					});
				}
			}
		}
		operator bool() const
		{
			return !v_parser.f_parsed() || v_painter.f_current() < v_rows.v_tokens.v_text.f_size();
		}
		void operator()()
		{
			auto size = v_rows.v_tokens.v_text.f_size();
			if (!v_parser.f_parsed()) {
				v_tokens.clear();
				v_tokens.emplace_back(A_NORMAL, size + 1);
			}
			for (size_t i = 0; i < c_task_unit; ++i) {
				uint32_t p;
				uint32_t n;
				uint32_t index;
				if (!v_parser.f_next(p, n, index)) {
					f_paint(size);
					v_painter.f_flush();
					v_creaser.f_end();
					v_message.clear();
					return;
				}
				v_captures[index](p, n);
			}
			v_painter.f_flush();
			std::wostringstream s;
			s << L"running: "sv << v_painter.f_current() * 100 / size << L'%';
			v_message = s.str();
		}

		nata::t_text_replaced v_replaced = [this](auto, auto, auto)
		{
			v_painter.f_reset();
			v_creaser.f_reset();
		};
	} syntax{view.v_rows};
	syntax.f_initialize();
	struct
	{
		decltype(*widget.f_overlays()[0].second) v_overlay;
		nata::t_painter<std::decay_t<decltype(v_overlay)>> v_painter{v_overlay};
		std::wregex v_pattern;
		std::regex_iterator<decltype(text.f_begin())> v_eos;
		std::regex_iterator<decltype(text.f_begin())> v_i;

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

		nata::t_text_replaced v_replaced = [this](auto, auto, auto)
		{
			v_i = decltype(v_i)(v_overlay.v_text.f_begin(), v_overlay.v_text.f_end(), v_pattern);
			v_painter.f_reset();
		};
	} search{*widget.f_overlays()[0].second};
	text.v_replaced >> search.v_replaced;
	search.v_replaced({}, {}, {});
	while (true) {
		view.f_render();
		size_t position = std::get<0>(widget.v_position);
		if (syntax.v_message.empty()) {
			auto line = text.f_lines().f_at_in_text(position).f_index();
			size_t column = position - line.v_i1;
			size_t x = std::get<1>(widget.v_position) - widget.v_line.v_x;
			size_t n = widget.f_range();
			std::wostringstream ss;
			ss << line.v_i0 << L',' << column << L'-' << x << L' ' << (n > 0 ? widget.v_top * 100 / n : 100) << L'%';
			auto s = ss.str();
			status.f_replace(0, status.f_size(), s.begin(), s.end());
		} else {
			status.f_replace(0, status.f_size(), syntax.v_message.begin(), syntax.v_message.end());
		}
		strip.f_render();
		view.v_target.f_cursor(std::get<1>(widget.v_position) - widget.v_row.f_index().v_x, widget.v_row.f_index().v_y - widget.v_top);
		view.v_target.f_timeout(syntax || search ? 0 : -1);
		wint_t c;
		if (view.v_target.f_get(c) != ERR) {
			if (c == 0x1b) break;
			auto clear_overlay = [&](auto& overlay)
			{
				nata::t_painter<std::decay_t<decltype(overlay)>> painter(overlay);
				painter.f_reset();
				painter.f_push(false, text.f_size());
				painter.f_flush();
			};
			switch (c) {
			case KEY_RESIZE:
				view.v_target.f_move(0, 0, COLS, LINES - 1);
				strip.v_target.f_move(0, LINES - 1, COLS, 1);
				break;
			case KEY_DOWN:
				if (widget.v_line.v_line < view.v_rows.f_size().v_line - 1) {
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
			case L'\b':
				if (position > 0) {
					view.v_rows.f_folded(--position, false);
					text.f_replace(position, 1, &c, &c);
				}
				break;
			case KEY_F(1):
				widget.f_position__(view.v_rows.f_folded(position, true), false);
				break;
			case KEY_F(2):
				view.v_rows.f_folded(position, false);
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
					search.v_replaced({}, {}, {});
				}
				break;
			case KEY_F(4):
				search.v_pattern = {};
				search.v_replaced({}, {}, {});
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
