EXTRA_DIST = test_spans.h test_rows.h test_nested.h
CLEANFILES = *.gcda *.gcno *.gcov

AM_CPPFLAGS = -I$(top_srcdir)/.. -I..
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

TEST_EXTENSIONS = .xm
AM_TESTS_ENVIRONMENT = export XEMMAI_MODULE_PATH=$(top_builddir)/xemmai:$(XEMMAI_MODULE_PATH);
XM_LOG_COMPILER = $(XEMMAI_BIN_PATH)
AM_XM_LOG_FLAGS = --verbose
scripts = \
	xemmai-text.xm \
	xemmai-search.xm
EXTRA_DIST = $(scripts)

TESTS = $(check_PROGRAMS) $(scripts)
