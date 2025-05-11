CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lreadline
TARGET = main
SOURCES = main.c parser.c executor.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS) # <--- This line MUST start with a TAB

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ # <--- This line MUST start with a TAB

clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: all clean