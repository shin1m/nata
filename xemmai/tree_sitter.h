#ifndef XEMMAIX__NATA__TREE_SITTER_H
#define XEMMAIX__NATA__TREE_SITTER_H

#include <tree_sitter/api.h>
#include "text.h"

namespace xemmaix::nata::tree_sitter
{

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

	static t_pvalue f_construct(t_type* a_class, const t_language& a_language, std::wstring_view a_source);

	t_query(const t_language& a_language, TSQuery* a_query) : v_module(a_language.v_module), v_language(a_language.v_language), v_query(a_query)
	{
	}
	virtual void f_destroy();
	t_pvalue f_captures() const;
};

struct t_parser : t_proxy
{
	t_text& v_text;
	decltype(*v_text.v_text) v_p;
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
			return v_p.f_at(p).f_base().f_index().v_byte;
		};
		TSInputEdit edit{byte(a_p), byte(a_p + a_n0), byte(a_p + a_n1)};
		ts_tree_edit(v_tree, &edit);
		v_parsed = false;
	};
	::nata::t_connection<decltype(v_replaced)>* v_connection;

	static t_pvalue f_construct(t_type* a_class, t_text& a_text, t_query& a_query)
	{
		return a_class->f_new<t_parser>(a_text, a_query);
	}

	t_parser(t_text& a_text, t_query& a_query) : v_text(a_text), v_p(*v_text.v_text), v_query(a_query), v_connection(v_p.v_replaced >> v_replaced)
	{
		v_text.f_acquire();
		v_query.f_acquire();
		ts_parser_set_language(v_parser, v_query.v_language);
	}
	virtual void f_destroy();
	bool f_parsed() const
	{
		return v_parsed;
	}
	t_pvalue f_next();
};

class t_library : public xemmai::t_library
{
	t_slot v_module_nata;
	xemmaix::nata::t_library* v_nata;
	t_slot_of<t_type> v_type_language;
	t_slot_of<t_type> v_type_query;
	t_slot_of<t_type> v_type_parser;

public:
	t_library(xemmai::t_library::t_handle* a_handle, const t_pvalue& a_nata) : xemmai::t_library(a_handle), v_module_nata(a_nata), v_nata(&v_module_nata->f_as<t_module>().v_body->f_as<xemmaix::nata::t_library>())
	{
	}
	virtual void f_scan(t_scan a_scan);
	virtual std::vector<std::pair<t_root, t_rvalue>> f_define();
	template<typename T>
	const T* f_library() const
	{
		return v_nata->f_library<T>();
	}
	template<typename T>
	t_slot_of<t_type>& f_type_slot()
	{
		return v_nata->f_type_slot<T>();
	}
	template<typename T>
	t_type* f_type() const
	{
		return const_cast<t_library*>(this)->f_type_slot<T>();
	}
	template<typename T>
	t_pvalue f_as(T&& a_value) const
	{
		using t = t_type_of<typename t_fundamental<T>::t_type>;
		return t::f_transfer(f_library<typename t::t_library>(), std::forward<T>(a_value));
	}
};

template<>
inline const t_library* t_library::f_library<t_library>() const
{
	return this;
}

XEMMAI__LIBRARY__TYPE(t_library, language)
XEMMAI__LIBRARY__TYPE(t_library, query)
XEMMAI__LIBRARY__TYPE(t_library, parser)

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
