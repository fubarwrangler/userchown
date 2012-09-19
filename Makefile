CFLAGS= -g -march=core2 -Wall -pedantic -Wextra -std=c99 -D_POSIX_C_SOURCE=200809L
INCLDIRS = include/
PREFIX= /usr

SOURCES=

all: phnxchown

phnxchown: phnxchown.o config.o permissions.o
	$(CC) -o phnxchown $^

%.o: %.c
	$(CC) $(CFLAGS) -I $(INCLDIRS) -c $^