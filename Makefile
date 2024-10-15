DEBUG = 0

CC = gcc

ifeq ($(DEBUG), 1)
	CFLAGS = -Og -g3
else
    CFLAGS = -O3 -march=native
endif

LDFLAGS = -Wall -Wextra

all:
	$(CC) -o BWTRLE bwtrle.c $(CFLAGS) $(LDFLAGS)
