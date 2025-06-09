link_libraries(nata)
add_executable(dump dump.cc)
add_library(examples-tree-sitter OBJECT
	${TREE_SITTER}/lib/src/lib.c
	${TREE_SITTER}-bash/src/scanner.cc
	${TREE_SITTER}-bash/src/parser.c
	${TREE_SITTER}-cpp/src/scanner.cc
	${TREE_SITTER}-cpp/src/parser.c
	${TREE_SITTER}-json/src/parser.c
)
target_include_directories(examples-tree-sitter PUBLIC ${TREE_SITTER}/lib/include)
add_executable(parse parse.cc)
target_link_libraries(parse examples-tree-sitter)
add_executable(curses curses.cc)
target_link_libraries(curses nata-curses examples-tree-sitter)
set_property(TARGET curses PROPERTY ENABLE_EXPORTS 1)
