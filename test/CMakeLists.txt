function(do_test name)
	add_executable(test_${name} test_${name}.cc)
	target_compile_options(test_${name} PRIVATE "-g;--coverage")
	target_link_libraries(test_${name} PRIVATE nata --coverage)
	add_test(NAME ${name} COMMAND test_${name})
endfunction()
do_test(signal)
do_test(stretches)
do_test(nested)
do_test(lines)
do_test(text)
do_test(tokens)
do_test(rows)
do_test(creases)
do_test(painter)
do_test(creaser)
function(do_test_xemmai name)
	add_test(NAME ${name} COMMAND xemmai --verbose "${CMAKE_CURRENT_SOURCE_DIR}/${name}.xm")
	set_property(TEST ${name} PROPERTY ENVIRONMENT "XEMMAI_MODULE_PATH=${PROJECT_BINARY_DIR}/xemmai")
endfunction()
do_test_xemmai(xemmai-text)
do_test_xemmai(xemmai-search)
do_test_xemmai(xemmai-tree-sitter)
