include config.mk

SRC=.
WLDSRC=wld

PKGS = fontconfig wayland-client xkbcommon pixman-1

WVKBD_SOURCES += $(wildcard $(SRC)/*.c)
WVKBD_HEADERS += $(wildcard $(SRC)/*.h)

CFLAGS += -std=gnu99 -Wall -g -DWITH_WAYLAND_SHM
CFLAGS += $(shell pkg-config --cflags $(PKGS))
LDFLAGS =wld/libwld.a $(shell pkg-config --libs $(PKGS)) -lm -lutil

WAYLAND_HEADERS = $(wildcard proto/*.xml)

HDRS = $(WAYLAND_HEADERS:.xml=-client-protocol.h)
WAYLAND_SRC = $(HDRS:.h=.c)
SOURCES = $(WVKBD_SOURCES) $(WAYLAND_SRC)

OBJECTS = $(SOURCES:.c=.o)

all: wld wvkbd

proto/%-client-protocol.c: proto/%.xml
	wayland-scanner code < $? > $@

proto/%-client-protocol.h: proto/%.xml
	wayland-scanner client-header < $? > $@

$(OBJECTS): $(HDRS) $(WVKBD_HEADERS)

wvkbd: $(OBJECTS)
	$(CC) -o wvkbd $(OBJECTS) $(LDFLAGS)

wld: wld/libwld.a

wld/libwld.a:
	$(MAKE) -C wld ENABLE_DRM=0

clean:
	rm -f $(OBJECTS) $(HDRS) $(WAYLAND_SRC) wvkbd
	$(MAKE) -C wld clean

format:
	clang-format -i $(WVKBD_SOURCES) $(WVKBD_HEADERS)
