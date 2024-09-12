# Variabili
CC = gcc
CFLAGS = -Wall -g
TARGET = my_program

# File sorgenti
SRCS = cinema.c main.c
# File oggetto
OBJS = $(SRCS:.c=.o)

# Regola principale
all: $(TARGET)

# Regola per creare l'eseguibile
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

# Regola per compilare i file .c in file .o
%.o: %.c cinema.h
	$(CC) $(CFLAGS) -c $< -o $@

# Regola per pulire i file oggetto e l'eseguibile
clean:
	rm -f $(OBJS) $(TARGET)
