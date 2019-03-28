CC			= gcc
SOURCES		= $(shell find . -maxdepth 1 -name '*.c')
OBJS		= $(SOURCES:.c=)
CFLAGS		= -lsqlite3
MAKEFLAGS   = --no-print-directory

%.c:
	$(CC) $(CFLAGS) $@.c -o $@

all:
	@make --no-print-directory clean
	@make --no-print-directory $(OBJS);

clean:
	rm -rf $(OBJS)
