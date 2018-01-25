xemmaidir = $(XEMMAI_MODULE_PATH)

xemmai_PROGRAMS = nata.so
AM_CPPFLAGS = -I$(top_srcdir)/.. $(XEMMAI_CFLAGS)
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
nata_so_LDADD = $(XEMMAI_LIBS) -lncursesw -ltinfo
nata_so_SOURCES = \
	text.h \
	text.cc \
	view.h \
	view.cc \
	search.h \
	search.cc \
	nata.h \
	nata.cc
