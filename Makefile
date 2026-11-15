CC      = gcc
CFLAGS  = -ansi -Wall -Wextra -pedantic
TARGET  = build/library
SOURCES = $(wildcard src/*.c)
HEADERS = $(wildcard src/*.h)

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $(SOURCES)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build

.PHONY: all run clean
