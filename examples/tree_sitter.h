#ifndef NATA__TREE_SITTER_H
#define NATA__TREE_SITTER_H

#include <tree_sitter/api.h>

namespace nata::tree_sitter
{

class t_query
{
	TSLanguage* v_language;
	TSQuery* v_query;

public:
	t_query(TSLanguage* a_language, std::string_view a_source) : v_language(a_language)
	{
		uint32_t error_offset;
		TSQueryError error_type;
		v_query = ts_query_new(a_language, a_source.data(), a_source.size(), &error_offset, &error_type);
		if (!v_query) throw std::make_pair(error_offset, error_type);
	}
	~t_query()
	{
		ts_query_delete(v_query);
	}
	operator TSLanguage*() const
	{
		return v_language;
	}
	operator TSQuery*() const
	{
		return v_query;
	}
};

template<typename T_text>
class t_parser
{
	T_text& v_text;
	t_query& v_query;
	TSParser* v_parser = ts_parser_new();
	char v_buffer[6];
	bool v_parsed = false;
	TSTree* v_tree = nullptr;
	TSQueryCursor* v_cursor = ts_query_cursor_new();

	::nata::t_slot<size_t, size_t, size_t> v_replaced = [this](auto a_p, auto a_n0, auto a_n1)
	{
		auto byte = [&](size_t p) -> uint32_t
		{
			return v_text.f_at(p).f_base().f_index().v_byte;
		};
		TSInputEdit edit{byte(a_p), byte(a_p + a_n0), byte(a_p + a_n1)};
		ts_tree_edit(v_tree, &edit);
		v_parsed = false;
	};

public:
	t_parser(T_text& a_text, t_query& a_query) : v_text(a_text), v_query(a_query)
	{
		v_text.v_replaced >> v_replaced;
		ts_parser_set_language(v_parser, v_query);
	}
	~t_parser()
	{
		ts_query_cursor_delete(v_cursor);
		ts_tree_delete(v_tree);
		ts_parser_delete(v_parser);
	}
	bool f_parsed() const
	{
		return v_parsed;
	}
	TSTree* f_parse()
	{
		if (v_parsed) return v_tree;
		auto old = v_tree;
		v_tree = ts_parser_parse(v_parser, old, {
			this,
			[](void* payload, uint32_t offset, TSPoint, uint32_t* read) -> const char*
			{
				auto* p = static_cast<t_parser*>(payload);
				auto& base = p->v_text.f_base();
				if (offset >= base.f_size().v_byte) {
					*read = 0;
					return nullptr;
				}
				auto i = base.f_at(offset);
				*read = i.f_tail() - &*i;
				auto d = jumoku::t_utf32_traits::f_delta(*i);
				if (*read >= d) return reinterpret_cast<const char*>(&*i);
				*read = d;
				std::copy_n(i, d, p->v_buffer);
				return p->v_buffer;
			},
			TSInputEncodingUTF8
		});
		ts_tree_delete(old);
		ts_query_cursor_exec(v_cursor, v_query, ts_tree_root_node(v_tree));
		v_parsed = true;
		return v_tree;
	}
	bool f_next(uint32_t& a_p, uint32_t& a_n, uint32_t& a_index)
	{
		f_parse();
		TSQueryMatch match;
		uint32_t index;
		if (!ts_query_cursor_next_capture(v_cursor, &match, &index)) return false;
		auto& capture = match.captures[index];
		auto character = [&](size_t p)
		{
			return v_text.f_base().f_at(p).f_index().v_character;
		};
		a_p = character(ts_node_start_byte(capture.node));
		a_n = character(ts_node_end_byte(capture.node)) - a_p;
		a_index = capture.index;
		return true;
	}
};

}

#endif
