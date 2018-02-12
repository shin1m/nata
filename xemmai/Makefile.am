xemmaidir = $(XEMMAI_MODULE_PATH)

xemmai_PROGRAMS = nata.so natacurses.so
AM_CPPFLAGS = -I$(top_srcdir)/.. $(XEMMAI_CFLAGS) $(NCURSES_CFLAGS)
AM_CXXFLAGS = -fPIC
AM_LDFLAGS = -shared
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
nata_so_LDADD = $(XEMMAI_LIBS)
nata_so_SOURCES = \
	proxy.h \
	proxy.cc \
	text.h \
	text.cc \
	search.h \
	search.cc \
	nata.h \
	nata.cc
natacurses_so_LDADD = $(XEMMAI_LIBS) $(NCURSES_LIBS)
natacurses_so_SOURCES = \
	view.h \
	painter.h \
	creaser.h \
	curses.h \
	curses.cc
