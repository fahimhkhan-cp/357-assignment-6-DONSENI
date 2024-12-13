CC = gcc
CFLAGS = -Wall -Wextra -std=c99

TARGET = Demo

all: $(TARGET)

$(TARGET): Demo.c
	$(CC) $(CFLAGS) Demo.c -o $(TARGET)

clean:
	rm -f $(TARGET)

