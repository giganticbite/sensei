CFLAGS=-std=c11 -g -static

sensei: sensei.c

test: sensei
	./test.sh

clean:
	rm -f sensei *.o *~ tmp*

.PHONY: test clean
