SOURCES = wish.c
HEADERS = wish.h

wish: $(SOURCES) $(HEADERS)
	gcc -o wish.out wish.c -I .