CC	= gcc

OUT	= app

SRC	= main.c terminal.c view.c
OBJ	= $(SRC:.c=.o)
HDR	= terminal.h terminal-private.h view.h view-private.h

CFLAGS	= -g -Wall
CFLAGS  += $(shell pkg-config gobject-2.0 --cflags)
LDFLAGS += $(shell pkg-config gobject-2.0 --libs)

all:	$(SRC) $(HDR) $(OUT)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ):

$(OUT): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	$(RM) $(OBJ) $(OUT) tags

tags:	force
	ctags *.[ch]

force:
