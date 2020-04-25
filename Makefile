CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

sensei: $(OBJS)
		$(CC) -o sensei $(OBJS) $(LDFLAGS)

$(OBJS): sensei.h

test: sensei
	./test.sh

clean:
	rm -f sensei *.o *~ tmp*

.PHONY: test clean
