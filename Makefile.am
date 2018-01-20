EXTRA_DIST = \
	signal.h \
	spans.h \
	stretches.h \
	nested.h \
	model.h \
	tokens.h \
	rows.h \
	widget.h \
	folder.h \
	curses.h
CLEANFILES = *.gcda *.gcno *.gcov
SUBDIRS = xemmai test

AM_CPPFLAGS = -I..
AM_CXXFLAGS =
AM_LDFLAGS =
if DEBUG
AM_CXXFLAGS += -g
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
curses_LDFLAGS = -lncursesw -ltinfo
