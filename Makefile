include config.mk

BUILDDIR=build-${LAYOUT}

NAME=wvkbd
BIN=${NAME}-${LAYOUT}
SRC=.
MAN1 = ${NAME}.1

PKGS = wayland-client xkbcommon pangocairo

WVKBD_SOURCES += $(wildcard $(SRC)/*.c)
WVKBD_HEADERS += $(wildcard $(SRC)/*.h)
WVKBD_DIR_SOURCES = $(foreach src, $(WVKBD_SOURCES), $(addprefix $(BUILDDIR)/, $(src)))

PKG_CONFIG ?= pkg-config
CFLAGS += -std=gnu99 -Wall -g -DWITH_WAYLAND_SHM -DLAYOUT=\"layout.${LAYOUT}.h\" -DKEYMAP=\"keymap.${LAYOUT}.h\"
CFLAGS += $(shell $(PKG_CONFIG) --cflags $(PKGS))
LDFLAGS += $(shell $(PKG_CONFIG) --libs $(PKGS)) -lm -lutil -lrt

WAYLAND_HEADERS = $(wildcard proto/*.xml)

HDRS = $(WAYLAND_HEADERS:.xml=-client-protocol.h)
WAYLAND_SRC = $(HDRS:.h=.c)
SOURCES = $(WVKBD_SOURCES) $(WAYLAND_SRC)
OBJECTS = $(WVKBD_DIR_SOURCES:.c=.o) $(WAYLAND_SRC:.c=.o)

SCDOC=scdoc
DOCS = wvkbd.1


all: ${BIN} ${DOCS}

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/config.h: $(BUILDDIR)
	cp config.$(LAYOUT).h $@

$(BUILDDIR)/%.o: $(BUILDDIR)
$(BUILDDIR)/%.o: %.c
	$(CC) -I $(CURDIR) -I $(CURDIR)/$(BUILDDIR) -c $(CFLAGS) -o $@ $<

proto/%-client-protocol.c: proto/%.xml
	wayland-scanner code < $? > $@

proto/%-client-protocol.h: proto/%.xml
	wayland-scanner client-header < $? > $@

$(OBJECTS): $(HDRS) $(WVKBD_HEADERS)

wvkbd-${LAYOUT}: $(BUILDDIR)/config.h $(OBJECTS) layout.${LAYOUT}.h
	$(CC) -o wvkbd-${LAYOUT} $(OBJECTS) $(LDFLAGS)

clean:
	rm -rf "$(BUILDDIR)" wvkbd-mobintl

format:
	clang-format -i $(WVKBD_SOURCES) $(WVKBD_HEADERS)

%: %.scd
	$(SCDOC) < $< > $@

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f ${NAME}-${LAYOUT} ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/${NAME}-${LAYOUT}
	mkdir -p "${DESTDIR}${MANPREFIX}/man1"
	sed "s/VERSION/${VERSION}/g" < ${MAN1} > ${DESTDIR}${MANPREFIX}/man1/${MAN1}
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/${MAN1}
