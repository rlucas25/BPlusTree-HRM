#include "Bplus.h"
#include <stdlib.h>
#include <stdbool.h>

Celula *criaCelula(void *info, void *chave) {
  if (!info || !chave) {
    erro("Parametros inconsistentes passados para criaCelula");
    return NULL;
  }

  Celula *novaCelula = mallocSafe(sizeof(Celula));
  novaCelula->chave = chave;
  novaCelula->info = info;
  novaCelula->parente = NULL;
}

Pagina *criaPagina(Celula *vetorCel, int tamanho, int ordem) {
  if ( !vetorCel ) {
    erro("Parametros inconsistentes passados para criaPagina"); // Verificar qual o min
    return NULL;
  }

  Pagina *pagina = mallocSafe(sizeof(Pagina));
  pagina->pag = mallocSafe(ordem * sizeof(Celula));
  pagina->qtdChaves = tamanho;
  pagina->ehFolha = false;

  for ( int i = 0; i < tamanho; i++){
    pagina->pag[i] = vetorCel[i];
  }

  return pagina;
}

Bplus *criaBplus(int ordem) {
  if (ordem < 1) {
    erro("Ordem menor do que 1 passada para criaBplus"); // Verificar qual o min
    return NULL;
  }

  Bplus *ArvoreBp = mallocSafe(sizeof(Bplus));
  ArvoreBp->ordem = ordem;
  ArvoreBp->raiz = NULL;

  return ArvoreBp;
}

Celula *buscaCelula(void *chave, Bplus *arvoreBplus, bool *encontrado, int (*comparaChave)(void *, void *)) {
  if ( !chave || !arvoreBplus || !encontrado || !comparaChave ) {
    erro("Parametros inconsistentes passados para buscaCelula");
    return NULL;
  }

  Pagina *p;
  int i;

  *encontrado = false;

  p = arvoreBplus->raiz->pag->parente;
  
  while(p != NULL){
    i=0;

    while( i < p->qtdChaves && comparaChave(chave, p->pag[i].chave) <= 0 ){
        i++;
    }

    if (p->ehFolha){
        // SE FOR IGUAL
        if(i < p->ehFolha && comparaChave(chave, p->pag[i].chave) == 0){

            *encontrado = true;
            return &p->pag[i];

        }
        if ( i == 0 ) return NULL;
        
        return &p->pag[i-1];

    }

    p = p->pag[i].parente;

  }
  
  return NULL;
}

void insereCelula(Bplus *arvoreBplus, void *info, void *chave) {
    if ( !arvoreBplus || !info || !chave ){
        erro("Parametros inconsistentes passados para insereCelula");
        return;
    }
    bool *encontrado = mallocSafe(sizeof(bool));

    Celula localCelula = buscaCelula(chave, arvoreBplus->raiz->pag[0], encontrado);

    if (encontrado) return;

    // FALTA IMPLEMENTAR
}

bool ehFolha(Pagina *pagina){
    if ( !pagina ){
        erro("Parametros inconsistentes passados para ehFolha");
    }
    return pagina->ehFolha; 
}

Pagina *cisaoPagina(Pagina * pagina, Bplus *arvoreBplus, void *info, void *chave){
    bool encontrado = false;
    Celula *resultado = buscaCelula(chave, pagina, encontrado);
    if(encontrado){
        return pagina;
    }
    Celula *novaCelula = criaCelula(info, chave);
    int tamPreCisao = pagina->qtdChaves;
    int tamTotal = tamPreCisao + 1;
   //tentar fazer pagina nova com tamanho equivalente
    insereCelula(arvoreBplus, chave, info);
    return pagina;
}