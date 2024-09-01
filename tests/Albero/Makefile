CFLAGS=-Wall -Wextra -g
TARGET=out
SRCS=albero.c coda.c
OBJS=$(SRCS:.c=.o)

%.o:%.c
	$(CC) $(CFLAGS) $< -c -o $@


all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

.PHONY:clean
clean:
	-$(RM) $(TARGET) $(OBJS)