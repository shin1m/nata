link_libraries(nata)
add_executable(dump dump.cc)
add_executable(curses curses.cc)
target_link_libraries(curses natacurses)
