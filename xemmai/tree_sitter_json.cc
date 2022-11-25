#include "tree_sitter.h"

extern "C" TSLanguage* tree_sitter_json();

namespace xemmaix::nata::tree_sitter::json
{

struct t_library : xemmai::t_library
{
	t_slot v_module_nata_tree_sitter;
	xemmaix::nata::tree_sitter::t_library* v_nata_tree_sitter;

	t_library(xemmai::t_library::t_handle* a_handle, const t_pvalue& a_nata_tree_sitter) : xemmai::t_library(a_handle), v_module_nata_tree_sitter(a_nata_tree_sitter), v_nata_tree_sitter(&v_module_nata_tree_sitter->f_as<t_module>().v_body->f_as<xemmaix::nata::tree_sitter::t_library>())
	{
	}
	XEMMAI__LIBRARY__MEMBERS
};

XEMMAI__LIBRARY__BASE(t_library, t_global, f_global())
XEMMAI__LIBRARY__BASE(t_library, xemmaix::nata::tree_sitter::t_library, v_nata_tree_sitter)

void t_library::f_scan(t_scan a_scan)
{
	a_scan(v_module_nata_tree_sitter);
}

std::vector<std::pair<t_root, t_rvalue>> t_library::f_define()
{
	return t_define(this)
		(L"language"sv, f_new<t_language>(this, t_object::f_of(this), tree_sitter_json()))
	;
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_library::t_handle* a_handle)
{
	using namespace std::literals;
	return xemmai::f_new<xemmaix::nata::tree_sitter::json::t_library>(a_handle, xemmai::t_module::f_instantiate(L"nata-tree-sitter"sv));
}
