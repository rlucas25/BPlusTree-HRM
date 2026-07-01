#include "Util.h"


void erro( const char *mensagem ){
  printf("\n\033[31mErro: %s\033[0m\n", mensagem);
}

void exito ( cost char *mensagem){
  printf("\n\033[32%s033[0m\n")
}

void *mallocSafe(size_t nbytes) {

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
