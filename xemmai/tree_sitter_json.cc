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
	virtual void f_scan(t_scan a_scan)
	{
		a_scan(v_module_nata_tree_sitter);
	}
	virtual std::vector<std::pair<t_root, t_rvalue>> f_define();
	template<typename T>
	const T* f_library() const
	{
		return v_nata_tree_sitter->f_library<T>();
	}
	template<typename T>
	t_slot_of<t_type>& f_type_slot()
	{
		return v_nata_tree_sitter->f_type_slot<T>();
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
