#include "Util.h"
#include <cstdio>
#include <string.h>

#define Nulo -1

 
typedef struct{
    long posRaiz;
    long ordem;
    size_t tamanhoChave;
    size_t tamanhoRegistro;
    long proxNolivre;
}CabecalhoBPlus;


typedef struct{
    long qtdChaves;
    
    void *chaves;
    void *ponteiros; // o tamanho precisa ser ordem + 1

    bool ehfolha;
    long proximaFolha; // Se for folha aponta para a folha seguinte
}PaginaBPlus;

bool criaArvore(const char *nomeArquivo, int ordem, size_t tamanhoChave, size_t tamanhoRegistro);
FILE *abrirArvore(const char *nomeArquivo, CabecalhoBPlus *cabecalho);
void fecharArvore(FILE *arquivoAberto);

long buscaChave(FILE *arquivoAberti, CabecalhoBPlus *cabecalho, const void *chave, int comparaChaves(void *x,void *y));

PaginaBPlus *cisaoPagina(PaginaBPlus *pagina, PaginaBPlus *paginaPai, CabecalhoBPlus *cabecalho, void* chavePromovida);
bool insereChave(FILE *arquivoAberto, CabecalhoBPlus *cabecalho, const void *chave, const void *registro, void comparaChaves(void *x,void *y));

bool removeChave(FILE *arquivoAberto, CabecalhoBPlus *cabecalho, const void *chave, void comparaChaves(void *x,void *y));
void concatenacao(FILE *arquivo, PaginaBPlus *atual, PaginaBPlus *irmao, const void *chavePai, CabecalhoBPlus *cabecalho);
PaginaBPlus redistribuicaoPagina();

void buscarIntervalo(FILE *arquivo, CabecalhoBPlus *cabecalho, const void *chaveA, const void *chaveB, int comparaChaves(void *x, void *y));
void BalancearArvoreUnderflow(FILE *arquivoAberto, CabecalhoBPlus *cabecalho, long posNoAtual, long *caminho, int tam_caminho);
