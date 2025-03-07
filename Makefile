# Define the compiler
CC = gcc

# Define the compilation flags
CFLAGS = -Wall -Wextra -I./src/lib -O2

# Define the linking flags
LDFLAGS = -lpthread

# Define directories
SRCDIR = ./src
LIBDIR = $(SRCDIR)/lib
OBJDIR = ./obj

# Find all .c files in the lib directory
LIBSOURCES = $(wildcard $(LIBDIR)/*.c)

# Generate object file names in the obj directory
LIBOBJECTS = $(patsubst $(LIBDIR)/%.c, $(OBJDIR)/%.o, $(LIBSOURCES))
CLIENT_OBJECT = $(OBJDIR)/client.o
SERVER_OBJECT = $(OBJDIR)/server.o

# Define the output executables
CLIENT_TARGET = client
SERVER_TARGET = server

# Default target (build both client and server)
all: $(OBJDIR) $(CLIENT_TARGET) $(SERVER_TARGET)

# Rule to create the object directory
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Rule to build the client executable
$(CLIENT_TARGET): $(LIBOBJECTS) $(CLIENT_OBJECT)
	$(CC) $(CFLAGS) -o $(CLIENT_TARGET) $^ $(LDFLAGS)

# Rule to build the server executable
$(SERVER_TARGET): $(LIBOBJECTS) $(SERVER_OBJECT)
	$(CC) $(CFLAGS) -o $(SERVER_TARGET) $^ $(LDFLAGS)

# Rule to compile client.c to obj/client.o
$(OBJDIR)/client.o: $(SRCDIR)/client.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to compile server.c to obj/server.o
$(OBJDIR)/server.o: $(SRCDIR)/server.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to compile library source files to obj/*.o
$(OBJDIR)/%.o: $(LIBDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule to remove all object files and the executables
clean:
	rm -rf $(OBJDIR) $(CLIENT_TARGET) $(SERVER_TARGET)

# Phony targets to avoid conflicts with files named 'clean'
.PHONY: clean all