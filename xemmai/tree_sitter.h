#ifndef XEMMAIX__NATA__TREE_SITTER_H
#define XEMMAIX__NATA__TREE_SITTER_H

#include <tree_sitter/api.h>
#include "text.h"

namespace xemmaix::nata::tree_sitter
{

template<typename C0, typename C1>
struct t_converter : private portable::t_iconv
{
	using portable::t_iconv::t_iconv;
	int f_to(char** a_p, size_t* a_n, auto a_out) const
	{
		char cs[16];
		char* p = cs;
		size_t n = sizeof(cs);
		while (iconv(v_cd, a_p, a_n, &p, &n) == size_t(-1)) {
			auto e = errno;
			if (e == EINTR) continue;
			a_out(reinterpret_cast<const C1*>(cs), (p - cs) / sizeof(C1));
			if (e != E2BIG) return e;
			p = cs;
			n = sizeof(cs);
		}
		a_out(reinterpret_cast<const C1*>(cs), (p - cs) / sizeof(C1));
		return 0;
	}
	std::basic_string<C1> operator()(std::basic_string_view<C0> a_x) const
	{
		std::basic_string<C1> s;
		auto p = reinterpret_cast<char*>(const_cast<C0*>(a_x.data()));
		size_t n = a_x.size() * sizeof(C0);
		auto append = [&](auto a_p, auto a_n)
		{
			s.append(a_p, a_p + a_n);
		};
		auto e = f_to(&p, &n, append);
		if (e == 0) e = f_to(nullptr, nullptr, append);
		if (e == 0) return s;
		throw std::system_error(e, std::generic_category());
	}
};

struct t_language
{
	t_slot v_module;
	TSLanguage* v_language;

	t_language(t_object* a_module, TSLanguage* a_language) : v_module(a_module), v_language(a_language)
	{
	}
};

struct t_query : t_proxy
{
	t_root v_module;
	TSLanguage* v_language;
	TSQuery* v_query;

	t_query(const t_language& a_language, TSQuery* a_query) : v_module(a_language.v_module), v_language(a_language.v_language), v_query(a_query)
	{
	}
	virtual void f_destroy();
};

struct t_parser : t_proxy
{
	t_text& v_text;
	t_query& v_query;
	TSParser* v_parser = ts_parser_new();
	char v_buffer[6];
	bool v_parsed = false;
	TSTree* v_tree = nullptr;
	TSQueryCursor* v_cursor = ts_query_cursor_new();

	::nata::t_text_replaced v_replaced = [this](auto a_p, auto a_n0, auto a_n1)
	{
		v_parsed = false;
		if (!v_tree) return;
		auto p = a_p.v_byte;
		TSInputEdit edit{static_cast<uint32_t>(p), static_cast<uint32_t>(p + a_n0.v_byte), static_cast<uint32_t>(p + a_n1.v_byte)};
		ts_tree_edit(v_tree, &edit);
	};
	::nata::t_connection<decltype(v_replaced)>* v_connection;

	t_parser(t_text& a_text, t_query& a_query) : v_text(a_text), v_query(a_query), v_connection(v_text.v_replaced >> v_replaced)
	{
		v_text.f_acquire();
		v_query.f_acquire();
		ts_parser_set_language(v_parser, v_query.v_language);
	}
	virtual void f_destroy();
	t_object* f_next();
};

class t_library : public xemmai::t_library
{
	t_slot v_module_nata;
	xemmaix::nata::t_library* v_nata;
#define XEMMAIX__NATA__TREE_SITTER__TYPES(_)\
	_(language)\
	_(query)\
	_(parser)
	XEMMAIX__NATA__TREE_SITTER__TYPES(XEMMAI__TYPE__DECLARE)

public:
	t_converter<wchar_t, char> v_to_utf8;
	t_converter<char, wchar_t> v_from_utf8;

	t_library(xemmai::t_library::t_handle* a_handle, const t_pvalue& a_nata) : xemmai::t_library(a_handle), v_module_nata(a_nata), v_nata(&v_module_nata->f_as<t_module>().v_body->f_as<xemmaix::nata::t_library>()), v_to_utf8("utf-8", "wchar_t"), v_from_utf8("wchar_t", "utf-8")
	{
	}
	XEMMAI__LIBRARY__MEMBERS
};

XEMMAI__LIBRARY__BASE(t_library, t_global, f_global())
XEMMAI__LIBRARY__BASE(t_library, xemmaix::nata::t_library, v_nata)
#define XEMMAI__TYPE__LIBRARY t_library
XEMMAIX__NATA__TREE_SITTER__TYPES(XEMMAI__TYPE__DEFINE)
#undef XEMMAI__TYPE__LIBRARY

struct t_language_library : xemmai::t_library
{
	t_slot v_module_nata_tree_sitter;
	xemmaix::nata::tree_sitter::t_library* v_nata_tree_sitter;
	TSLanguage* v_language;

	t_language_library(xemmai::t_library::t_handle* a_handle, const t_pvalue& a_nata_tree_sitter, TSLanguage* a_language) : xemmai::t_library(a_handle), v_module_nata_tree_sitter(a_nata_tree_sitter), v_nata_tree_sitter(&v_module_nata_tree_sitter->f_as<t_module>().v_body->f_as<xemmaix::nata::tree_sitter::t_library>()), v_language(a_language)
	{
	}
	XEMMAI__LIBRARY__MEMBERS
};

XEMMAI__LIBRARY__BASE(t_language_library, t_global, f_global())
XEMMAI__LIBRARY__BASE(t_language_library, xemmaix::nata::tree_sitter::t_library, v_nata_tree_sitter)

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::nata::tree_sitter::t_language> : t_uninstantiatable<t_holds<xemmaix::nata::tree_sitter::t_language>>
{
	using t_library = xemmaix::nata::tree_sitter::t_library;

	static void f_define(t_library* a_library);

	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		a_scan(a_this->f_as<xemmaix::nata::tree_sitter::t_language>().v_module);
	}
};

template<>
struct t_type_of<xemmaix::nata::tree_sitter::t_query> : t_derivable<t_bears<xemmaix::nata::tree_sitter::t_query, t_type_of<xemmaix::nata::t_proxy>>>
{
	using t_library = xemmaix::nata::tree_sitter::t_library;

	static void f_define(t_library* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

template<>
struct t_type_of<xemmaix::nata::tree_sitter::t_parser> : t_derivable<t_bears<xemmaix::nata::tree_sitter::t_parser, t_type_of<xemmaix::nata::t_proxy>>>
{
	using t_library = xemmaix::nata::tree_sitter::t_library;

	static void f_define(t_library* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif
