# Define the compiler
CC = gcc

# Define the flags
CFLAGS = -Wall -Wextra -I./src/lib

# Define the source directories
SRCDIR = ./src
LIBDIR = $(SRCDIR)/lib

# Define the output executable
TARGET = program

# Find all .c files in the lib directory and main.c
LIBSOURCES = $(wildcard $(LIBDIR)/*.c)
MAINSOURCE = $(SRCDIR)/main.c

# Generate object file names for libraries
LIBOBJECTS = $(LIBSOURCES:.c=.o)

# The first rule is the default target, i.e., the final executable
$(TARGET): $(LIBOBJECTS) $(SRCDIR)/main.o
	$(CC) $(CFLAGS) -o $(TARGET) $^

# Rule to build main.o
$(SRCDIR)/main.o: $(MAINSOURCE)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to build each lib .o file
$(LIBDIR)/%.o: $(LIBDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule to remove all object files and the executable
clean:
	rm -f $(LIBDIR)/*.o $(SRCDIR)/main.o $(TARGET)

# Phony targets to avoid conflicts with files named 'clean'
.PHONY: clean
