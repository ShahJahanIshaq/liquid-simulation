CC = cc
CFLAGS = -std=c99 -Wall -O2
CFLAGS += $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs)

TARGET = main
SRC = src/main.c src/input.c src/renderer.c src/simulate.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all run clean