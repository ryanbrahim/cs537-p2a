SOURCES = wish.c
HEADERS = wish.h

wish: $(SOURCES) $(HEADERS)
	gcc -o wish wish.c -I .