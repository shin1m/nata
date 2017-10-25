EXTRA_DIST = \
	spans.h \
	signal.h \
	model.h \
	view.h \
	test_spans.h

CLEANFILES = *.gcda *.gcno *.gcov

AM_CPPFLAGS = -I..
AM_CXXFLAGS =
AM_LDFLAGS =
if DEBUG
AM_CXXFLAGS += -g -fprofile-arcs -ftest-coverage
else
AM_CPPFLAGS += -DNDEBUG
AM_CXXFLAGS += -O3
endif
if PROFILE
AM_CXXFLAGS += -pg
AM_LDFLAGS += -pg
endif
AM_DEFAULT_SOURCE_EXT = .cc
bin_PROGRAMS = \
	dump \
	curses
curses_LDFLAGS = -lncursesw
check_PROGRAMS = \
	test_stretches \
	test_signal \
	test_lines \
	test_text \
	test_view
TESTS = $(check_PROGRAMS)
