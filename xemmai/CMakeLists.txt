function(add_module name)
	add_library(${name}-xemmai MODULE ${ARGN})
	target_link_libraries(${name}-xemmai PRIVATE ${name} xemmai)
	set_target_properties(${name}-xemmai PROPERTIES OUTPUT_NAME ${name} PREFIX "")
	install(TARGETS ${name}-xemmai DESTINATION lib/xemmai-${xemmai_VERSION_MAJOR})
endfunction()
add_module(nata proxy.cc text.cc search.cc nata.cc)
add_module(nata-curses curses.cc)
add_library(nata-tree-sitter INTERFACE)
target_include_directories(nata-tree-sitter INTERFACE ${TREE_SITTER}/lib/include)
target_link_libraries(nata-tree-sitter INTERFACE nata)
add_module(nata-tree-sitter tree_sitter.cc ${TREE_SITTER}/lib/src/lib.c)
function(add_tree_sitter_language name)
	file(GENERATE OUTPUT tree_sitter_${name}.cc CONTENT "\
#include \"tree_sitter.h\"
extern \"C\" TSLanguage* tree_sitter_${name}();
XEMMAI__MODULE__FACTORY(xemmai::t_library::t_handle* a_handle)
{
	using namespace std::literals;
	return xemmai::f_new<xemmaix::nata::tree_sitter::t_language_library>(a_handle, xemmai::t_module::f_instantiate(L\"nata-tree-sitter\"sv), tree_sitter_${name}());
}
")
	set(target nata-tree-sitter-${name}-xemmai)
	file(GLOB source ${TREE_SITTER}-${name}/src/*.c*)
	add_library(${target} MODULE ${CMAKE_CURRENT_BINARY_DIR}/tree_sitter_${name}.cc ${source})
	target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
	target_link_libraries(${target} PRIVATE nata-tree-sitter xemmai)
	set_target_properties(${target} PROPERTIES OUTPUT_NAME nata-tree-sitter-${name} PREFIX "")
	install(TARGETS ${target} DESTINATION lib/xemmai-${xemmai_VERSION_MAJOR})
endfunction()
add_tree_sitter_language(bash)
add_tree_sitter_language(cpp)
add_tree_sitter_language(json)
