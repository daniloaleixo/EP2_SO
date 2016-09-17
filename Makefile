CC = gcc
CFLAGS = -Wall -g -O0 -pedantic
LFLAGS = -lpthread
OUT = ep2
OBJS = ep2.o auxiliares.o

all: clean $(OUT)

clean:
	rm -f $(OUT) $(OBJS)

ep2: auxiliares.o
	$(CC) ep2.c auxiliares.o $(LFLAGS) -o ep2

auxiliares.o: auxiliares.c
	$(CC) auxiliares.c -c -o auxiliares.o
