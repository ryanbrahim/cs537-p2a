SOURCES = wish.c
HEADERS = 

wish: $(SOURCES) $(HEADERS)
	gcc -o wish wish.c -I .

test: wish
	~cs537-1/tests/p2a/test-wish.sh

test-all: wish
	~cs537-1/tests/p2a/test-wish.sh -c

