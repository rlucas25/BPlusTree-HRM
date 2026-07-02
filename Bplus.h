#ifndef Bplus_H
#define Bplus_H

#include "Util.h"
#include <string.h>

#define Nulo -1

typedef struct {
    
    long posRaiz;
    long ordemP;
    size_t tamanhoChave;
    size_t tamanhoRegistro;
    long proxNolivre;
} CabecalhoBPlus;

typedef struct {

    long qtdChaves;
    void *chaves;
    void *ponteiros; 
    bool ehfolha;
    long proximaFolha; 
} PaginaBPlus;

int maxChavesPagina(CabecalhoBPlus *cabecalho, bool ehfolha);
int metadeArredondada(CabecalhoBPlus *cabecalho);
int minChavesPagina(CabecalhoBPlus *cabecalho, bool ehfolha);

// função responsavel por criar a arvore.
bool criaArvore(const char *nomeArquivo, int ordem, size_t tamanhoChave, size_t tamanhoRegistro);

// função responsavel por abrir a arvore.
FILE *abrirArvore(const char *nomeArquivo, CabecalhoBPlus *cabecalho);

// função responsavel por ler a pagina do disco.
PaginaBPlus le_pagina_disco(FILE *arquivo, long offset, CabecalhoBPlus *cabecalho);

// função responsavel por fechar a arvore.
void fecharArvore(FILE *arquivoAberto);

// função responsavel por buscar chave.
long buscarChave(FILE *arquivoAberto, bool *encontrou, CabecalhoBPlus *cabecalho, void *chave,
                 int (*comparaChaves)(void *x, void *y), long *caminho, int *tam_caminho);

// função responsavel pela cisão no processo de inserção.
PaginaBPlus *cisaoPagina(PaginaBPlus *pagina, PaginaBPlus *paginaPai, CabecalhoBPlus *cabecalho,
                         void *chavePromovida);

// função responsavel pela insersção de uma chave na arvore                       
bool insereChave(FILE *arquivoAberto, CabecalhoBPlus *cabecalho, void *chave, const void *registro,
                 int (*comparaChaves)(void *x, void *y));

// função responsável por alocar páginas em espaços livres
long alocarPagina(FILE *arquivoAberto, CabecalhoBPlus *cabecalho);

// função responsavel pela remoção de uma chave na arvore
bool removeChave(FILE *arquivoAberto, CabecalhoBPlus *cabecalho, void *chave,
                 int (*comparaChaves)(void *x, void *y));

// função reponsavel pela concatenacao de paginas na arvore
void concatenacao(FILE *arquivo, PaginaBPlus *atual, PaginaBPlus *irmao, const void *chavePai,
                  CabecalhoBPlus *cabecalho);

// função responsavel pela redistribuilçao de chaves na arvore
PaginaBPlus redistribuicaoPagina(PaginaBPlus *atual, PaginaBPlus *irmao, bool ehEsq, int idxFilhoAtual,
                    PaginaBPlus *pai, CabecalhoBPlus *cabecalho);

// função responsavel pelo balanceamento apos a retirada de um elemento na arvore
void BalancearArvoreUnderflow(FILE *arquivoAberto, CabecalhoBPlus *cabecalho, long posNoAtual,
                              long *caminho, int tam_caminho);

// função responsavel por imprimir a pagina formatada
void imprimePagina(PaginaBPlus pagina, CabecalhoBPlus *cabecalho,
                   void (*imprimechave)(void *chave));

// função responsavel por imprimir a arvore sformatada                   
void imprimeBPlus(FILE *arquivo, CabecalhoBPlus *cabecalho, long posisaoPagina, const char *prefixo,
                  bool ehUltimo, void (*imprimeNo)(void *chave));
#endif