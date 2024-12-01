# Define the compiler
CC = gcc

# Define the flags (adding -lpthread and -O2)
CFLAGS = -Wall -Wextra -I./src/lib -lpthread -O2

# Define the source directories
SRCDIR = ./src
LIBDIR = $(SRCDIR)/lib

# Find all .c files in the lib directory
LIBSOURCES = $(wildcard $(LIBDIR)/*.c)

# Generate object file names for libraries
LIBOBJECTS = $(LIBSOURCES:.c=.o)

# Define the output executables
CLIENT_TARGET = client
SERVER_TARGET = server

# Default target (build both client and server)
all: $(CLIENT_TARGET) $(SERVER_TARGET)

# Rule to build the client executable (removed duplicate -lpthread since it's in CFLAGS)
$(CLIENT_TARGET): $(LIBOBJECTS) $(SRCDIR)/client.o
	$(CC) $(CFLAGS) -o $(CLIENT_TARGET) $^

# Rule to build the server executable (removed duplicate -lpthread since it's in CFLAGS)
$(SERVER_TARGET): $(LIBOBJECTS) $(SRCDIR)/server.o
	$(CC) $(CFLAGS) -o $(SERVER_TARGET) $^

# Rule to build client.o
$(SRCDIR)/client.o: $(SRCDIR)/client.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to build server.o
$(SRCDIR)/server.o: $(SRCDIR)/server.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to build each lib .o file
$(LIBDIR)/%.o: $(LIBDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule to remove all object files and the executables
clean:
	rm -f $(LIBDIR)/*.o $(SRCDIR)/*.o $(CLIENT_TARGET) $(SERVER_TARGET)

# Phony targets to avoid conflicts with files named 'clean'
.PHONY: clean all