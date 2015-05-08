### Architecture choice

# Uncomment for 64-bit
LIBRARY = libtiptap.a

# Uncomment for 32-bit
#LIBRARY = libtiptap32.a


### Other flags

# Language features (C99 etc.)
CFLAGS += -std=c99 -D_DEFAULT_SOURCE

# Warning and error settings
CFLAGS += -Wall -Wextra
CFLAGS += -Werror -Wno-error=sign-compare -Wno-error=unused-function -Wno-error=unused-label -Wno-error=unused-parameter 

# Debugging information
CFLAGS += -g


### File lists

BINS = simulate
OBJS = simulate.o driver.o


### Phony targets all/clean

.PHONY: all clean

all: $(BINS)

clean:
	$(RM) $(BINS) $(OBJS)


### Build rules

# Be sure to link in the library
$(BINS): $(OBJS) $(LIBRARY)

# Add header file dependencies
driver.o: driver.h tiptap.h
simulate.o: driver.h tiptap.h
