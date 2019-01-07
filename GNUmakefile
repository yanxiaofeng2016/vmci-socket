# Change to 1 to build windows test programs
WINDOWS := 0

VMCI_CFLAGS := $(shell ./vmci-cflags)
CFLAGS := -Wall -Os -g $(VMCI_CFLAGS)

.PHONY: all clean

PROGRAMS := socket client

ifeq ($(WINDOWS),1)
PROGRAMS += socket.exe client.exe
endif

DERIVED_FILES := $(PROGRAMS)

all: $(DERIVED_FILES)

%: %.c sock-posix.c
	$(CC) $+ $(CFLAGS) $(LDFLAGS) -o $@
%: LDFLAGS += $(VMCI_LIBS)

%.exe: %.c sock-windows.c
	$(CC) $+ $(CFLAGS) $(LDFLAGS) -o $@
%.exe: CC := i686-w64-mingw32-gcc
%.exe: LDFLAGS += $(VMCI_LIBS) -lws2_32

clean:
	rm -f $(DERIVED_FILES)
