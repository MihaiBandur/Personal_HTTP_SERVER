### Configuration. ####################################################
# Default C compiler options.
CFLAGS = -Wall -g
# C source files for the server.
SOURCES = server.c module.c common.c main.c
# Corresponding object files.
OBJECTS = $(SOURCES:.c=.o)
# Server module shared library files.
MODULES = diskfree.so issue.so processes.so time.so
### Rules. ############################################################
# Phony targets don’t correspond to files that are built; they’re names
# for conceptual build targets.
.PHONY: all clean
# Default target: build everything.
all: server $(MODULES)
# Clean up build products.
clean:
rm -f $(OBJECTS) $(MODULES) server
# The main server program. Link with -Wl,-export-dyanamic so
# dynamically loaded modules can bind symbols in the program. Link in
# libdl, which contains calls for dynamic loading.
server: $(OBJECTS)
$(CC) $(CFLAGS) -Wl,-export-dynamic -o $@ $^ -ldl
# All object files in the server depend on server.h. But use the
# default rule for building object files from source files.
$(OBJECTS): server.h
# Rule for building module shared libraries from the corresponding
# source files. Compile -fPIC and generate a shared object file.
$(MODULES): \
%.so: %.c server.h
$(CC) $(CFLAGS) -fPIC -shared -o $@ $<
