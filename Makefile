CFLAGS = -Wall -Wextra -Werror -pedantic -std=c99 -Wno-unused-command-line-argument
LDFLAGS = -lm

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

all: CFLAGS += -O2
all: bell202

debug: CFLAGS += -O0 -g -fsanitize=address -fsanitize=undefined -DDEBUG
debug: bell202

bell202: $(OBJS)
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -c -o $@

.PHONY: clean
clean:
	rm -rf bell202 *.o *.dSYM 
