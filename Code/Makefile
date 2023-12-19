# Makefile for crtp.c

CC = gcc
CFLAGS = -Wall -Werror
LDFLAGS = -lhiredis

SRC = main.c
TARGET = crtp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
