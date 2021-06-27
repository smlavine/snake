.POSIX:

include config.mk

CFILES = main.c
HFILES = rogueutil.h

all: options snake

options:
	@echo "CFLAGS      = $(CFLAGS)"
	@echo "OPTIMFLAGS  = $(OPTIMFLAGS)"
	@echo "DEBUGFLAGS  = $(DEBUGFLAGS)"

snake: $(CFILES) $(HFILES)
	$(CC) -o $@ $(CFILES) $(CFLAGS) $(OPTIMFLAGS)

debug: $(CFILES) $(HFILES)
	$(CC) -o snake-$@ $(CFILES) $(CFLAGS) $(DEBUGFLAGS)

clean:
	rm -f snake snake-debug

install: snake
	mkdir -p $(PREFIX)/bin
	cp -f snake $(PREFIX)/bin
	chmod 755 $(PREFIX)/bin
	mkdir -p $(MANPREFIX)/man1
	sed "s/VERSION/$(VERSION)/g" < snake.1 > $(MANPREFIX)/man1/snake.1
	chmod 644 $(MANPREFIX)/man1/snake.1

uninstall:
	rm -f $(PREFIX)/bin/snake\
		$(PREFIX)/man1/snake.1

.PHONY: all options snake debug clean install uninstall
