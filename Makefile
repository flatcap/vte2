CC	= gcc
LN	= ln -s

OUT	= app

SRC	= main.c terminal.c view.c
OBJ	= $(SRC:.c=.o)
HDR	= terminal.h terminal-private.h view.h view-private.h
LINKS	= gobject gtk

CFLAGS	= -g -Wall
CFLAGS  += $(shell pkg-config gobject-2.0 --cflags)
LDFLAGS += $(shell pkg-config gobject-2.0 --libs)

all:	$(SRC) $(HDR) $(OUT) $(LINKS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ):

$(OUT): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	$(RM) $(OBJ) $(OUT) tags $(LINKS)

tags:	force
	ctags *.[ch]

force:

gobject:
	$(LN) /usr/src/dev/glib-2.22.5/gobject

gtk:
	$(LN) /usr/src/dev/gtk+-2.18.9/gtk

