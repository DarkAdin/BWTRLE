DEBUG = 0
OPENMP = 1

CC = gcc

ifeq ($(DEBUG), 1)
	CFLAGS = -Og -g3
else
    CFLAGS = -Ofast -march=native
endif

ifeq ($(OPENMP), 1)
    LDFLAGS = -Wall -Wextra -fopenmp
else
    LDFLAGS = -Wall -Wextra
endif

all:
	$(CC) -o BWTRLE bwtrle.c $(CFLAGS) $(LDFLAGS)
