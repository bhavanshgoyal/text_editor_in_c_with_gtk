# Makefile for Advanced Text Editor with GTK

CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0` -Wall -Wextra
LDFLAGS = `pkg-config --libs gtk+-3.0`
TARGET = text_editor
SRC = text_editor.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

install-deps:
	@echo "Installing GTK+ 3 development libraries..."
	@echo "On Ubuntu/Debian: sudo apt-get install libgtk-3-dev"
	@echo "On Fedora: sudo dnf install gtk3-devel"
	@echo "On Arch: sudo pacman -S gtk3"

.PHONY: all clean run install-deps
