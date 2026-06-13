TARGET = game.exe
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wno-missing-field-initializers
LIBS = -lgdi32 -lmsimg32
LDFLAGS = -mwindows
SOURCES = main.c core.c ui.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LIBS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	cmd /c del /Q $(subst /,\,$(OBJECTS) $(TARGET))