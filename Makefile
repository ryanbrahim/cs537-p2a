SOURCES = wish.c
HEADERS = wish.h

wish: $(SOURCES) $(HEADERS)
	gcc -o out/wish wish.c -I .