#include <nata/model.h>
#include <nata/rows.h>
#include <nata/widget.h>
#include <nata/painter.h>
#include <nata/creaser.h>
#include <nata/curses.h>
#include "tree_sitter.h"
#include <filesystem>
#include <fstream>
#include <regex>
#include <dlfcn.h>

using namespace std::literals;

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

std::regex pattern_pair("\\s*([^:]+?)\\s*:\\s*(.*)");

std::map<std::string, std::string> f_read_pairs(const std::filesystem::path& a_path)
{
	std::map<std::string, std::string> pairs;
	std::ifstream in(a_path);
	auto c = in.get();
	while (c != EOF) {
		std::vector<char> line;
		while (true) {
			if (c == '\r') c = in.get();
			if (c == EOF) break;
			if (c == '\n') {
				c = in.get();
				break;
			}
			line.push_back(c);
			c = in.get();
		}
		std::match_results<std::vector<char>::iterator> match;
		if (std::regex_match(line.begin(), line.end(), match, pattern_pair)) pairs.emplace(match[1], match[2]);
	}
	return pairs;
}

std::filesystem::path syntax_root;

std::pair<std::unique_ptr<nata::tree_sitter::t_query>, std::map<std::string, std::string>> f_try_syntax(t_text& a_text, const std::string_view a_path, const std::filesystem::path& a_type)
{
	auto detect = f_read_pairs(a_type / "detect"sv);
	auto match = false;
	if (!a_path.empty()) {
		auto i = detect.find("path");
		if (i != detect.end() && std::regex_search(a_path.begin(), a_path.end(), std::regex(i->second))) match = true;
	}
	if (!match) {
		auto i = detect.find("content");
		if (i != detect.end()) {
			std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
			if (std::regex_search(a_text.f_begin(), a_text.f_end(), std::wregex(convert.from_bytes(i->second)))) match = true;
		}
	}
	if (!match) return {};
	auto handle = dlopen(NULL, RTLD_LAZY);
	if (!handle) throw std::runtime_error("dlopen");
	auto language = dlsym(handle, ("tree_sitter_" + a_type.filename().string()).c_str());
	dlclose(handle);
	if (!language) throw std::runtime_error(dlerror());
	std::string query;
	{
		std::ifstream in(a_type / "query.scm"sv);
		query.assign(std::istreambuf_iterator<char>(in), {});
	}
	return {std::make_unique<nata::tree_sitter::t_query>(reinterpret_cast<TSLanguage*(*)()>(language)(), query), f_read_pairs(a_type / "colors"sv)};
}

std::pair<std::unique_ptr<nata::tree_sitter::t_query>, std::map<std::string, std::string>> f_load_syntax(t_text& a_text, const std::string_view a_path)
{
	for (auto& type : std::filesystem::directory_iterator(syntax_root)) {
		auto [query, capture2color] = f_try_syntax(a_text, a_path, type);
		if (query) return {std::move(query), std::move(capture2color)};
	}
	return {};
}

const std::map<std::string_view, attr_t> color2token{
	{"red"sv, COLOR_PAIR(4)},
	{"green"sv, COLOR_PAIR(5)},
	{"yellow"sv, COLOR_PAIR(6)},
	{"blue"sv, COLOR_PAIR(7)},
	{"magenta"sv, COLOR_PAIR(8)},
	{"cyan"sv, COLOR_PAIR(9)}
};

struct t_syntax
{
	decltype(t_view::v_rows)& v_rows;
	std::unique_ptr<nata::tree_sitter::t_query> v_query;
	nata::t_painter<decltype(t_view::v_tokens)> v_painter{v_rows.v_tokens};
	nata::t_creaser<decltype(t_view::v_rows)> v_creaser{v_rows};
	nata::tree_sitter::t_parser<t_text> v_parser{v_rows.v_tokens.v_text, *v_query};
	std::vector<std::pair<attr_t, uint32_t>> v_tokens;
	std::vector<std::function<void(uint32_t, uint32_t)>> v_captures;

	nata::t_text_replaced v_replaced = [this](auto, auto, auto)
	{
		v_painter.f_reset();
		v_creaser.f_reset();
	};

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
	t_syntax(decltype(t_view::v_rows)& a_rows, std::unique_ptr<nata::tree_sitter::t_query>&& a_query, const std::map<std::string, std::string>& a_capture2color) : v_rows(a_rows), v_query(std::move(a_query))
	{
		v_rows.v_tokens.v_text.v_replaced >> v_replaced;
		auto count = ts_query_capture_count(*v_query);
		for (size_t i = 0; i < count; ++i) {
			uint32_t n;
			auto p = ts_query_capture_name_for_id(*v_query, i, &n);
			std::string_view name(p, n);
			if (name == "crease"sv) {
				v_captures.emplace_back([&](uint32_t p, uint32_t n)
				{
					v_creaser.f_push(p - v_creaser.f_current(), n);
				});
			} else {
				auto token = A_NORMAL;
				auto i = a_capture2color.find(std::string(name));
				if (i != a_capture2color.end()) {
					auto j = color2token.find(i->second);
					if (j != color2token.end()) token = j->second;
				}
				v_captures.emplace_back([&, token](uint32_t p, uint32_t n)
				{
					f_paint(p);
					v_tokens.emplace_back(token, p + n);
				});
			}
		}
	}
	bool operator()()
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
				return false;
			}
			v_captures[index](p, n);
		}
		v_painter.f_flush();
		return true;
	}
};

constexpr attr_t attribute_selected = A_REVERSE;
constexpr attr_t attribute_highlighted = COLOR_PAIR(3);

int main(int argc, char* argv[])
{
	std::locale::global(std::locale(""));
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
	auto path = argc > 1 ? argv[1] : nullptr;
	if (path) {
		std::wifstream in(path);
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
	std::wstring message;
	std::vector<std::pair<std::chrono::time_point<std::chrono::steady_clock>, std::function<void()>>> timers;
	std::vector<std::function<void()>> tasks;
	{
		syntax_root = std::filesystem::path(argv[0]).parent_path().parent_path() / "syntax"sv;
		auto [query, capture2color] = f_load_syntax(text, path ? path : ""sv);
		if (query) tasks.emplace_back([&, syntax = std::make_shared<t_syntax>(view.v_rows, std::move(query), capture2color)]
		{
			if (!(*syntax)()) return;
			std::wostringstream s;
			s << L"running: "sv << syntax->v_painter.f_current() * 100 / text.f_size() << L'%';
			message = s.str();
			timers.emplace_back(std::chrono::steady_clock::now(), []{});
		});
	}
	struct
	{
		decltype(*widget.f_overlays()[0].second) v_overlay;
		nata::t_painter<std::decay_t<decltype(v_overlay)>> v_painter{v_overlay};
		std::wregex v_pattern;
		std::regex_iterator<decltype(text.f_begin())> v_eos;
		std::regex_iterator<decltype(text.f_begin())> v_i;

		bool operator()()
		{
			auto size = v_overlay.v_text.f_size();
			if (v_painter.f_current() >= size) return false;
			for (size_t i = 0; i < c_task_unit; ++i) {
				if (v_i == v_eos) break;
				auto& m = (*v_i)[0];
				v_painter.f_push(false, m.first.f_index() - v_painter.f_current(), 64);
				v_painter.f_push(true, m.second.f_index() - m.first.f_index(), 64);
				++v_i;
			}
			if (v_i == v_eos) v_painter.f_push(false, size - v_painter.f_current(), 64);
			v_painter.f_flush();
			return true;
		}

		nata::t_text_replaced v_replaced = [this](auto, auto, auto)
		{
			v_i = decltype(v_i)(v_overlay.v_text.f_begin(), v_overlay.v_text.f_end(), v_pattern);
			v_painter.f_reset();
		};
	} search{*widget.f_overlays()[0].second};
	text.v_replaced >> search.v_replaced;
	search.v_replaced({}, {}, {});
	tasks.emplace_back([&]
	{
		if (search()) timers.emplace_back(std::chrono::steady_clock::now(), []{});
	});
	while (true) {
		auto now = std::chrono::steady_clock::now();
		{
			auto ts = std::move(timers);
			for (auto& x : ts)
				if (x.first > now)
					timers.push_back(x);
				else
					x.second();
		}
		for (auto& x : tasks) x();
		size_t position = std::get<0>(widget.v_position);
		if (message.empty()) {
			auto line = text.f_lines().f_at_in_text(position).f_index();
			size_t column = position - line.v_i1;
			size_t x = std::get<1>(widget.v_position) - widget.v_line.v_x;
			size_t n = widget.f_range();
			std::wostringstream ss;
			ss << line.v_i0 << L',' << column << L'-' << x << L' ' << (n > 0 ? widget.v_top * 100 / n : 100) << L'%';
			auto s = ss.str();
			status.f_replace(0, status.f_size(), s.begin(), s.end());
		} else {
			status.f_replace(0, status.f_size(), message.begin(), message.end());
			message.clear();
		}
		widget.f_into_view(widget.v_row);
		view.f_render();
		strip.f_render();
		view.v_target.f_cursor(std::get<1>(widget.v_position) - widget.v_row.f_index().v_x, widget.v_row.f_index().v_y - widget.v_top);
		auto timeout = -1;
		for (auto& x : timers) {
			auto t = (x.first - now) / 1ms;
			if (timeout < 0 || t < timeout) timeout = t;
		}
		view.v_target.f_timeout(timeout);
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
		}
	}
	return 0;
}
