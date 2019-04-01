CC			= gcc
SOURCES		= $(shell find . -maxdepth 1 -name '*.c')
EXECS		= client server
OBJS		= $(SOURCES:.c=.o)
CFLAGS		= -g
LDFLAGS		= -lsqlite3
MAKEFLAGS	= --no-print-directory

.PHONY: all clean

%.c: %.h

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

all: $(EXECS)

clean:
	rm -f $(OBJS) $(EXECS) *.txt *.png

client: client.o common.o input_parser.o
	$(CC) $^ $(LDFLAGS) -o $@

server: server.o common.o
	$(CC) $^ $(LDFLAGS) -o $@
