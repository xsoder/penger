PREFIX ?= /usr/local
DESTDIR ?=
BINDIR := $(PREFIX)/bin

CFLAGS := -Wall -Wextra -Iraylib-5.5_linux_amd64/include
LDFLAGS := -Lraylib-5.5_linux_amd64/lib -l:libraylib.a -lm

SRC := penger.c
BIN := penger

RAYLIB_DIR := raylib-5.5_linux_amd64
RAYLIB_TAR := $(RAYLIB_DIR).tar.gz

.PHONY: all install uninstall clean $(RAYLIB_DIR)

all: $(BIN)

$(BIN): $(SRC) $(RAYLIB_DIR)
	mkdir -p "$(HOME)/.fonts"
	cp -f Iosevka-Regular.ttf "$(HOME)/.fonts/"
	gcc $(CFLAGS) -o $@ $< $(LDFLAGS)

$(RAYLIB_DIR):
	@if [ ! -d "$(RAYLIB_DIR)" ]; then \
		wget https://github.com/raysan5/raylib/releases/download/5.5/$(RAYLIB_TAR); \
		tar -xf $(RAYLIB_TAR); \
		rm -f $(RAYLIB_TAR); \
	fi

clean:
	-rm -f $(BIN)
	-rm -rf $(RAYLIB_DIR) $(RAYLIB_TAR)

install: $(BIN)
	install -d "$(DESTDIR)$(BINDIR)"
	install -m 0755 "$(BIN)" "$(DESTDIR)$(BINDIR)"

uninstall:
	-rm -f "$(DESTDIR)$(BINDIR)/$(BIN)"
