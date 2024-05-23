CC = gcc
CFLAGS = -Wall -Wextra

main: ./src/main.c
	$(CC) $(CFLAGS) -o main ./src/main.c

clean:
	rm -f main