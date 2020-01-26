# Advanced Data Structures

CC=gcc

SRCS=$(shell printf "%s " src/*.c)
OBJS=$(subst .c,,$(SRCS))

src/%: src/%.c
	$(CC) -o $@ $<

all: $(OBJS)

.PHONY: clean

clean:
	find ./src -type f ! -name '*.[ch]' -delete
