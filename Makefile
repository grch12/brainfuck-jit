PROJECT_NAME = bfjit

CC = gcc

C_SOURCES = $(wildcard *.c)
S_SOURCES = $(wildcard *.s)

C_OBJS = $(patsubst %.c, %.o, $(C_SOURCES))
S_OBJS = $(patsubst %.s, %.o, $(S_SOURCES))

CFLAGS = -Wall -Wextra -Wpedantic -Ofast
ASFLAGS =

.PHONY: all clean rebuild

all: $(PROJECT_NAME)

$(PROJECT_NAME): $(C_OBJS) $(S_OBJS)
	gcc $(C_OBJS) $(S_OBJS) -o $@ -s

clean:
	rm -f $(PROJECT_NAME) $(C_OBJS) $(S_OBJS)

rebuild: clean all
