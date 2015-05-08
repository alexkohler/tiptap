### Architecture choice

# Uncomment for 64-bit
LIBRARY = libtiptap.a

# Uncomment for 32-bit
#LIBRARY = libtiptap32.a


### Other flags

# Language features (C99 etc.)
CFLAGS += -std=c99 -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L -D_BSD_SOURCE -D_XOPEN_SOURCE

# Warning and error settings
CFLAGS += -Wall -Wextra
CFLAGS += -Werror -Wno-error=sign-compare -Wno-error=type-limits
CFLAGS += -Wno-unused-variable -Wno-unused-function -Wno-unused-label -Wno-unused-parameter -Wno-unused-but-set-variable

# Debugging information
CFLAGS += -g


### File lists

BINS = simulate a4verify
OBJS = simulate.o driver.o model.o a4verify.o


### Phony targets all/clean

.PHONY: all clean

all: $(BINS)

clean:
	$(RM) $(BINS) $(OBJS)


### Build rules

# Be sure to link in the driver and library
simulate: driver.o $(LIBRARY)
a4verify: model.o driver.o $(LIBRARY)

# Add header file dependencies
driver.o: driver.h tiptap.h
simulate.o: driver.h tiptap.h
a4verify.o: a4verify.h driver.h model.h
