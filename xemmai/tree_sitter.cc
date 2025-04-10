#include "tree_sitter.h"
#include <codecvt>
#include <locale>

namespace xemmai
{

void t_type_of<xemmaix::nata::tree_sitter::t_language>::f_define(t_library* a_library)
{
	using xemmaix::nata::tree_sitter::t_language;
	t_define{a_library}.f_derive<t_language, t_object>();
}

void t_type_of<xemmaix::nata::tree_sitter::t_query>::f_define(t_library* a_library)
{
	using xemmaix::nata::tree_sitter::t_query;
	t_define{a_library}
		(L"captures"sv, t_member<t_object*(*)(const t_query&), [](auto a_this)
		{
			return t_tuple::f_instantiate(ts_query_capture_count(a_this.v_query), [&](auto& tuple)
			{
				std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
				for (size_t i = 0; i < tuple.f_size(); ++i) {
					uint32_t n;
					auto p = ts_query_capture_name_for_id(a_this.v_query, i, &n);
					new(&tuple[i]) t_svalue(t_string::f_instantiate(convert.from_bytes(p, p + n)));
				}
			});
		}>())
	.f_derive<t_query, xemmaix::nata::t_proxy>();
}

namespace
{

const wchar_t* v_query_errors[] = {
	L"none",
	L"syntax",
	L"node type",
	L"field",
	L"capture",
	L"structure",
	L"language"
};

}

t_pvalue t_type_of<xemmaix::nata::tree_sitter::t_query>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_object*(*)(t_type*, const xemmaix::nata::tree_sitter::t_language&, std::wstring_view), [](auto a_class, auto a_language, auto a_source)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
		auto source = convert.to_bytes(a_source.data(), a_source.data() + a_source.size());
		uint32_t error_offset;
		TSQueryError error_type;
		if (auto query = ts_query_new(a_language.v_language, source.data(), source.size(), &error_offset, &error_type)) return a_class->template f_new<xemmaix::nata::tree_sitter::t_query>(a_language, query);
		f_throw(L"ts_query_new: "s + std::to_wstring(error_offset) + L": "s + (static_cast<size_t>(error_type) > TSQueryErrorLanguage ? L"unknown" : v_query_errors[error_type]));
	}>::t_bind<xemmaix::nata::tree_sitter::t_query>::f_do(this, a_stack, a_n);
}

void t_type_of<xemmaix::nata::tree_sitter::t_parser>::f_define(t_library* a_library)
{
	using xemmaix::nata::tree_sitter::t_parser;
	t_define{a_library}
		(L"parsed"sv, t_member<bool(*)(const t_parser&), [](auto a_this)
		{
			return a_this.v_parsed;
		}>())
		(L"next"sv, t_member<t_object*(t_parser::*)(), &t_parser::f_next>())
	.f_derive<t_parser, xemmaix::nata::t_proxy>();
}

t_pvalue t_type_of<xemmaix::nata::tree_sitter::t_parser>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_object*(*)(t_type*, xemmaix::nata::t_text&, xemmaix::nata::tree_sitter::t_query&), [](auto a_class, auto a_text, auto a_query)
	{
		return a_class->template f_new<xemmaix::nata::tree_sitter::t_parser>(a_text, a_query);
	}>::t_bind<xemmaix::nata::tree_sitter::t_parser>::f_do(this, a_stack, a_n);
}

}

namespace xemmaix::nata::tree_sitter
{

void t_query::f_destroy()
{
	ts_query_delete(v_query);
	v_module = nullptr;
}

void t_parser::f_destroy()
{
	delete v_connection;
	ts_query_cursor_delete(v_cursor);
	ts_tree_delete(v_tree);
	ts_parser_delete(v_parser);
	v_query.f_release();
	v_text.f_release();
}

t_object* t_parser::f_next()
{
	if (!v_parsed) {
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
		ts_query_cursor_exec(v_cursor, v_query.v_query, ts_tree_root_node(v_tree));
		v_parsed = true;
	}
	TSQueryMatch match;
	uint32_t index;
	if (!ts_query_cursor_next_capture(v_cursor, &match, &index)) return nullptr;
	auto& capture = match.captures[index];
	auto character = [&](size_t p)
	{
		return v_text.f_base().f_at(p).f_index().v_character;
	};
	auto p = character(ts_node_start_byte(capture.node));
	return f_tuple(p, character(ts_node_end_byte(capture.node)) - p, capture.index);
}

void t_library::f_scan(t_scan a_scan)
{
	a_scan(v_module_nata);
	a_scan(v_type_language);
	a_scan(v_type_query);
	a_scan(v_type_parser);
}

std::vector<std::pair<t_root, t_rvalue>> t_library::f_define()
{
	t_type_of<t_language>::f_define(this);
	t_type_of<t_query>::f_define(this);
	t_type_of<t_parser>::f_define(this);
	return t_define(this)
		(L"Language"sv, static_cast<t_object*>(v_type_language))
		(L"Query"sv, static_cast<t_object*>(v_type_query))
		(L"Parser"sv, static_cast<t_object*>(v_type_parser))
	;
}

void t_language_library::f_scan(t_scan a_scan)
{
	a_scan(v_module_nata_tree_sitter);
}

std::vector<std::pair<t_root, t_rvalue>> t_language_library::f_define()
{
	return t_define(this)
		(L"language"sv, f_new<t_language>(this, t_object::f_of(this), v_language))
	;
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_library::t_handle* a_handle)
{
	using namespace std::literals;
	return xemmai::f_new<xemmaix::nata::tree_sitter::t_library>(a_handle, xemmai::t_module::f_instantiate(L"nata"sv));
}
