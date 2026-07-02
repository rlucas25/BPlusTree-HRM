#include "Util.h"

void erro(const char *mensagem) {

    // exibe mensagem de erro formatada
    printf("\n\033[31mErro: %s\033[0m\n", mensagem);
}

void exito(const char *mensagem) {

    // exibe mensagem de sucesso formatada
    printf("\n\033[32m%s\033[0m\n", mensagem);
}

void *mallocSafe(size_t nbytes) {

    // Verifica se o tamanho solicitado é válido (maior que zero)
    if (nbytes <= 0) {
        erro("Tentativa de alocar tamanho invalido (zero ou negativo)");
        exit(1);
    }
    void *ptr = malloc(nbytes);

    if (ptr == NULL) {
        erro("Alocação de memoria");
        exit(1);
    }

    return ptr;
}
