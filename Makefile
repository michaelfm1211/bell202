CFLAGS = -Wall -Wextra -Werror -pedantic -std=c99

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

all: CFLAGS += -O3
all: bell202

debug: CFLAGS += -O0 -g -fsanitize=address -fsanitize=undefined
debug: bell202

bell202: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

.PHONY: clean
clean:
	rm -rf bell202 *.o *.dSYM 
