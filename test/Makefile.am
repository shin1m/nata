EXTRA_DIST = test_spans.h test_rows.h test_nested.h
CLEANFILES = *.gcda *.gcno *.gcov

AM_CPPFLAGS = -I../.. -I..
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
check_PROGRAMS = \
	test_signal \
	test_stretches \
	test_nested \
	test_lines \
	test_text \
	test_tokens \
	test_rows \
	test_creases \
	test_painter \
	test_creaser
TESTS = $(check_PROGRAMS)
