ifeq ($(origin PREFIX),default)
    INSTALL_LOCAL := 1
    BINDIR := .
else
    INSTALL_LOCAL := 0
    BINDIR := $(DESTDIR)$(PREFIX)/bin
endif

CFLAGS := -Wall -Wextra -Iraylib-5.5_linux_amd64/include $(shell pkg-config --cflags glib-2.0 libnotify)
LDFLAGS := -Lraylib-5.5_linux_amd64/lib -l:libraylib.a -lm $(shell pkg-config --libs glib-2.0 libnotify)

SRC := penger.c
BIN := penger

RAYLIB_DIR := raylib-5.5_linux_amd64
RAYLIB_TAR := $(RAYLIB_DIR).tar.gz

.PHONY: all install uninstall clean $(RAYLIB_DIR) install-assets

all: $(BIN)

$(BIN): $(SRC) $(RAYLIB_DIR) install-assets
	gcc $(CFLAGS) -o $@ $< $(LDFLAGS)

$(RAYLIB_DIR):
	@if [ ! -d "$(RAYLIB_DIR)" ]; then \
		wget https://github.com/raysan5/raylib/releases/download/5.5/$(RAYLIB_TAR); \
		tar -xf $(RAYLIB_TAR); \
		rm -f $(RAYLIB_TAR); \
	fi

install: $(BIN)
	@if [ "$(INSTALL_LOCAL)" = "0" ]; then \
		mkdir -p "$(BINDIR)"; \
	fi
	@install -m 0755 "$(BIN)" "$(BINDIR)/$(BIN)"

install-assets:
	@if [ ! -f "$(HOME)/.fonts/Iosevka-Regular.ttf" ]; then \
		mkdir -p "$(HOME)/.fonts"; \
		cp -f Iosevka-Regular.ttf "$(HOME)/.fonts/"; \
	fi
	@if [ ! -f "$(HOME)/.local/audio/bip.mp3" ]; then \
		mkdir -p "$(HOME)/.local/audio"; \
		cp -f bip.mp3 "$(HOME)/.local/audio/"; \
	fi

clean:
	-rm -f $(BIN)
	-rm -rf $(RAYLIB_DIR) $(RAYLIB_TAR)

uninstall:
	-rm -f "$(BINDIR)/$(BIN)"
