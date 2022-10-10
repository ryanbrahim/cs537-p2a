SOURCES = wish.c
HEADERS = wish.h

wish.out: $(SOURCES) $(HEADERS)
	gcc -o wish wish.c -I .