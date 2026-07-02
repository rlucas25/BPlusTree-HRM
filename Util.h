#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// função responsavel por exibir mensagem de erro
void erro(const char *mensagem);
// função responsavel por exibir mensagem de sucesso
void exito(const char *mensagem);
// função responsavel por alocar memória de forma segura
void *mallocSafe(size_t nbytes);
