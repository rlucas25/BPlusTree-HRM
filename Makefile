C = gcc
EXECUTAVEL = SistemaRH.exe

CFLAGS = -O0 -std=c11 -Wall -Werror -Wextra -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable -Wshadow -fsanitize=address,undefined,leak 
CLIBS = -lm
LDFLAGS = -fsanitize=address,undefined,leak

SRCS = main.c \
       Bplus.c \
       RH.c \
       Util.c
       
OBJS = $(SRCS:.c=.o)

all: $(EXECUTAVEL)

$(EXECUTAVEL): $(OBJS)
	$(C) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $(EXECUTAVEL) $(CLIBS)
	@echo "Compilação concluída com sucesso!"

%.o: %.c
	$(C) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXECUTAVEL)
	@echo "Diretório limpo!"
    
run: $(EXECUTAVEL)
	@echo "Executando o programa..."
	@ASAN_OPTIONS=detect_leaks=1 ./$(EXECUTAVEL)