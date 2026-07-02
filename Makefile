TARGET = SistemaRH

C = gcc

CFLAGS=-O0 -std=c11 -Wall -Werror -Wextra -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable -Wshadow -fsanitize=address,undefined,leak 

SRCS = main.c \
       Bplus.c \
       RH.c \
	   Util.c \

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(C) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c
	$(C) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all shortcut clean