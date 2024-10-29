CFLAGS=-std=c11 -g -static
SRCS=$(filter-out test.c, $(wildcard *.c))
OBJS=$(SRCS:.c=.o)

9cc: $(OBJS)
	$(CC) -o 9cc $(OBJS) $(LDFLAGS)

$(OBJS): 9cc.h

test: 9cc
	./test.sh

clean:
	rm -f 9cc *.o *~ tmp* *.out

gdb: 9cc
	gdb ./9cc

tmp:
	gcc -static -g -o tmp tmp.s

.PHONY: test clean tmp