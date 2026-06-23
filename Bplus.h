#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "util.h"

typedef struct Celula Celula;
typedef struct Pagina Pagina;
typedef struct Bplus Bplus;

typedef struct Node {
    int* keys;
    int t; 
    struct Node** children;
    int n; 
    bool ehFolha; 
    struct Node* next; 
} Node;

// typedef struct NoBPlus {
//     int *chaves;                  // Array para guardar as chaves
//     struct NoBPlus **filhos;      // Array de ponteiros para os nós filhos
//     int numChaves;                // Quantidade atual de chaves no nó
//     bool ehFolha;                 // Flag que indica se o nó é uma folha
//     struct NoBPlus *proximoFolha; // Ponteiro para a próxima folha (exclusivo da B+)
// } NoBPlus;

struct Celula {
  void *info;    // Ponteiro para a info real
  void *chave;   // Usada para ordenar
  Pagina *parente; // Aponta para os menores ou iguais a chave ou a pagina irma
};

struct Pagina {
  int qtdChaves; // Quantidade de chaves na pagina
  Celula *pag;   // vetor de celulas
  bool ehFolha;
};

struct Bplus {
  Pagina *raiz;
  int ordem;
}; 

Celula *criaCelula(void *info, void *chave);
Pagina *criaPagina(Celula *vetorCel, int tamanho);
Bplus *criaBplus(int ordem);

Celula *buscaCelula(void *chave, Bplus *arvoreBplus, bool *encontrado, int (*comparaChave)(void *, void *))
bool ehFolha(Pagina *pagina);
void insereCelula(Bplus *arvoreBplus, void *info, void *chave);
Pagina *cisaoPagina(Pagina *pag, Bplus *arvoreBplus, void *info, void *chave);
Celula *removeCelula(Celula celula);
Pagina *concatenacaoPagina(Pagina pag);
Pagina *redistribuicaoPagina(Celula celula);
void liberaPagina(Pagina *p);
void liberaBplus(Bplus arvore);


