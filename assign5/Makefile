### Architecture choice

ifeq ($(shell uname -m),x86_64)
LIBRARIES = libtiptap.a libttnet.a
else
LIBRARIES = libtiptap32.a libttnet32.a
endif


### Other flags

# Language features (C99 etc.)
CFLAGS += -std=c99 -pthread -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L -D_BSD_SOURCE -D_XOPEN_SOURCE

# Warning and error settings
CFLAGS += -Wall -Wextra
CFLAGS += -Werror -Wno-error=sign-compare -Wno-error=type-limits
CFLAGS += -Wno-unused-variable -Wno-unused-function -Wno-unused-label -Wno-unused-parameter -Wno-unused-but-set-variable

# Debugging information
CFLAGS += -g


### File lists

BINS = server simulate
OBJS = server.o simulate.o driver.o network.o


### Phony targets all/clean/tarball

.PHONY: all clean tarball

all: $(BINS)

clean:
	$(RM) $(BINS) $(OBJS)

tarball:
	tar -cvzf ../submission-$(USER)-$(shell basename $(PWD)).tar.gz -C.. $(shell basename $(PWD))
	@echo
	@echo Tarball has been created as ../submission-$(USER)-$(shell basename $(PWD)).tar.gz
	@echo Please rename it to the format given in the assignment before submitting!


### Build rules

# Be sure to link in the library
simulate: driver.o network.o
server: driver.o network.o

simulate server: LDLIBS+=$(LIBRARIES)

# Add header file dependencies
network.o: network.h
