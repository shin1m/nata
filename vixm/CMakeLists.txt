function(do_test_xemmai name)
	add_test(NAME vixm-${name} COMMAND xemmai --verbose "${CMAKE_CURRENT_SOURCE_DIR}/${name}.xm")
	set_property(TEST vixm-${name} PROPERTY ENVIRONMENT "XEMMAI_MODULE_PATH=${PROJECT_BINARY_DIR}/xemmai")
endfunction()
do_test_xemmai(test)
