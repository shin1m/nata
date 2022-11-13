#include <nata/model.h>
#include <string>
#include "tree_sitter.h"

extern "C" TSLanguage* tree_sitter_json();

void f_walk(TSTreeCursor& a_cursor, size_t a_indent)
{
	auto node = ts_tree_cursor_current_node(&a_cursor);
	std::printf("%*s[%d, %d) %s\n", a_indent, "", ts_node_start_byte(node), ts_node_end_byte(node), ts_node_type(node));
	if (ts_tree_cursor_goto_first_child(&a_cursor)) {
		++a_indent;
		do f_walk(a_cursor, a_indent); while (ts_tree_cursor_goto_next_sibling(&a_cursor));
		ts_tree_cursor_goto_parent(&a_cursor);
	}
};

int main(int argc, char* argv[])
{
	using namespace std::literals;
	std::setlocale(LC_ALL, "");
	nata::t_text<nata::t_lines<5, 5>, 5, 5> text;
	auto replace = [&](auto p, auto n, auto s)
	{
		text.f_replace(p, n, s.begin(), s.end());
	};
	replace(0, 0, L"[1, null, \"\u00a3\u0939\u20ac\U00010348\"]"sv);
	try {
		nata::tree_sitter::t_query query(tree_sitter_json(), R"scm(
("{" @begin_object)
("}" @end_object)
("[" @begin_array)
("]" @end_array)
([(object) (array)] @crease)
)scm"sv);
		nata::tree_sitter::t_parser parser(text, query);
		auto dump = [&]
		{
			std::printf("%ls\n", std::wstring(text.f_begin(), text.f_end()).c_str());
			size_t j = 0;
			for (auto i = text.f_base().f_begin(); i != text.f_base().f_end(); ++i, ++j) std::printf(" %d:%02x", j, *i);
			std::printf("\n");
			auto root = ts_tree_root_node(parser.f_parse());
			{
				auto cursor = ts_tree_cursor_new(root);
				f_walk(cursor, 1);
				ts_tree_cursor_delete(&cursor);
			}
			auto list = [&]
			{
				uint32_t p;
				uint32_t n;
				uint32_t index;
				while (parser.f_next(p, n, index)) {
					uint32_t m;
					auto name = ts_query_capture_name_for_id(query, index, &m);
					std::printf(" [%d, %d) %s\n", p, n, std::string(name, m).c_str());
				}
			};
			list();
			for (auto& x : parser.v_ranges)
			{
				std::printf(" changed: %d, %d\n", x.first, x.second);
				auto node = ts_node_descendant_for_byte_range(root, x.first, x.second);
				auto cursor = ts_tree_cursor_new(node);
				f_walk(cursor, 2);
				ts_tree_cursor_delete(&cursor);
				parser.f_reset(node);
				list();
				parser.f_reset(node, x.first, x.second);
				list();
			}
		};
		dump();
		replace(4, 4, L"{\"\u00a3\u0939\": \"\u20ac\U00010348\"}"sv);
		dump();
		replace(11, 4, L"null"sv);
		dump();
		replace(11, 5, L"2}"sv);
		dump();
		replace(11, 2, L""sv);
		dump();
		replace(11, 0, L"false}"sv);
		dump();
		return 0;
	} catch (std::pair<uint32_t, TSQueryError>& error) {
		std::fprintf(stderr, "query error: %d, %d\n", error.first, error.second);
		return 1;
	}
}
