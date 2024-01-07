# define varaibles
CC=gcc
LEVFLAGS=-lev -Wl,-rpath  # for libev
# define target & source
SOURCES=status.c db.c dbobj.c dict.c murmurhash.c dlist.c str.c sortedSet.c
OBJS=$(SOURCES:.c=.o)
TARGET=libdatabase.so

.PHONY: all clean

# generate dynamic shared library
# $^: all dependency
# $@: target
$(TARGET): $(OBJS)
	gcc -shared -g -o $(TARGET) $(OBJS) $(LEVFLAGS)

# generate .o files
# $<: first dependencies
# $@: target
%.o: %.c
	gcc -fPIC -g -c $< -o $@

# clean target and *.o files
clean:
	rm -f $(OBJS) $(TARGET)