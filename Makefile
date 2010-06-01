#-------------------------------------------------------------------------------
# Commands
#
AWK	= awk
CAT	= cat
CC	= gcc
CP	= cp
GEN_GGM	= glib-genmarshal
GEN_GME	= glib-mkenums
GPERF	= gperf
GREP	= grep
LN	= ln -s
MKDIR	= mkdir -p
MV	= mv
NM	= nm
RM	= rm -fr
SED	= sed
SORT	= LANG=C sort

#-------------------------------------------------------------------------------
# File lists
#
OUT	= app

SRC	= caps.c debug.c iso2022.c main.c matcher.c pty.c reaper.c ring.c \
	  rseq-vte.c table.c terminal.c trie.c view.c vteconv.c vterowdata.c \
	  vtestream.c vtetc.c vtetree.c vteunistr.c
OBJ	= $(SRC:.c=.o)
HDR	= buffer.h caps.h config.h debug.h gdk_keysyms.h gnome-pty.h iso2022.h \
	  matcher.h pty.h reaper.h ring.h screen.h table.h terminal.h \
	  terminal-private.h trie.h view.h view-private.h vteconv.h vtepty.h \
	  vtepty-private.h vterowdata.h vtestream-base.h vtestream-file.h \
	  vtestream.h vtetc.h vtetree.h vteunistr.h

# Generated source, to be compiled
GEN_SRC	= marshal.c vtetypebuiltins.c
GEN_HDR	= marshal.h vtetypebuiltins.h rperf-vte.c

SRC	+= $(GEN_SRC)
HDR	+= $(GEN_HDR)

LINKS	= gobject gtk src symbols work unitable

#-------------------------------------------------------------------------------
# Compilation flags
#
# General compiler flags
CFLAGS	+= -g
#CFLAGS	+= -O2
CFLAGS	+= -fno-common
#CFLAGS	+= -fprofile-arcs
#CFLAGS	+= -ftest-coverage

# Extra definitions
CFLAGS	+= -DDATADIR='"/home/dev/install/share"'
#CFLAGS	+= -DG_DISABLE_ASSERT
CFLAGS	+= -DG_LOG_DOMAIN='"Vte"'
CFLAGS	+= -D_GNU_SOURCE
CFLAGS	+= -DHAVE_CONFIG_H
CFLAGS	+= -DLIBEXECDIR='"/home/dev/install/libexec"'
CFLAGS	+= -DLOCALEDIR='"/home/dev/install/share/locale"'
CFLAGS	+= -DVTE_COMPILATION
#CFLAGS	+= -DVTE_DEBUG
CFLAGS	+= -DRARXXX

# Warning flags
CFLAGS	+= -Waggregate-return
CFLAGS	+= -Wall
CFLAGS	+= -Wcast-align
#CFLAGS	+= -Wdeclaration-after-statement
CFLAGS	+= -Werror-implicit-function-declaration
CFLAGS	+= -Wextra
CFLAGS	+= -Wimplicit
CFLAGS	+= -Winit-self
#CFLAGS	+= -Wmissing-declarations
CFLAGS	+= -Wmissing-format-attribute
#CFLAGS	+= -Wmissing-prototypes
CFLAGS	+= -Wnested-externs
CFLAGS	+= -Wno-missing-field-initializers
CFLAGS	+= -Wno-switch-enum
CFLAGS	+= -Wno-unused-parameter
CFLAGS	+= -Wold-style-definition
CFLAGS	+= -Wpacked
CFLAGS	+= -Wpointer-arith
CFLAGS	+= -Wshadow
CFLAGS	+= -Wsign-compare
CFLAGS	+= -Wstrict-aliasing=2
CFLAGS	+= -Wstrict-prototypes
CFLAGS	+= -Wunsafe-loop-optimizations
CFLAGS	+= -Wwrite-strings

# Basic include dirs
CFLAGS	+= -I.
CFLAGS	+= -Iunitable
CFLAGS	+= $(shell pkg-config gobject-2.0 gio-unix-2.0 --cflags)
LDFLAGS	+= $(shell pkg-config gobject-2.0 gio-unix-2.0 --libs)

#-------------------------------------------------------------------------------
# Build targets
#
all:	$(SRC) $(HDR) $(LINKS) tags $(OUT)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ):

$(OUT): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	$(RM) $(OBJ) $(OUT)

distclean: clean
	$(RM) tags*
	$(RM) $(LINKS)
	$(RM) $(GEN_SRC) $(GEN_HDR)
	$(RM) unitable

tags:	force
	ctags *.[ch] remnants/*.[ch]

force:

supertags:
	ctags *.[ch] src/*.[ch]

#-------------------------------------------------------------------------------
# Symlinks, etc
#
links:	$(LINKS)

gobject:
	$(LN) /usr/src/dev/glib-2.24.1/gobject

gtk:
	$(LN) /usr/src/dev/gtk+-2.20.1/gtk

symbols:
	$(LN) /home/dev/notes/symbols

src:
	$(LN) /home/dev/git.vte/src $@

work:
	$(LN) /home/dev/vte $@

unitable:
	$(MKDIR) $@
	$(CP) -l /home/dev/git.vte/src/unitable.* $@

#-------------------------------------------------------------------------------
# Generated files
#
marshal.c: marshal.list
	$(GEN_GGM) --prefix=_vte_marshal --header --body --internal $^ > $@

marshal.h: marshal.list
	$(GEN_GGM) --prefix=_vte_marshal --header --internal $^ > $@

vtetypebuiltins.c: vtepty.h
	$(GEN_GME) --template vtetypebuiltins.c.template vtepty.h > $@

vtetypebuiltins.h: vtepty.h
	$(GEN_GME) --template vtetypebuiltins.h.template vtepty.h > $@

rperf-vte.c:
	$(GPERF) -m 100 --no-strlen rperf-vte.gperf > $@

