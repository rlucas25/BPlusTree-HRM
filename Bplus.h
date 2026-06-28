#include "Util.h"
#include <cstdio>

//extern char nomeArquivo[256] = NULL;
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

long buscaChave(FILE *arquivoAberti, CabecalhoBPlus *cabecalho, const void *chave, void comparaChaves(void *x,void *y));

PaginaBPlus *cisaoPagina(PaginaBPlus *pagina, CabecalhoBPlus *cabecalho, void* chavePromovida);
bool insereChave(FILE *arquivoAberto,bool *encontrou, CabecalhoBPlus *cabecalho, const void *chave, const void *registro, void comparaChaves(void *x,void *y));

bool removeChave(FILE *arquivoAberto, CabecalhoBPlus *cabecalho, const void *chave, void comparaChaves(void *x,void *y));
PaginaBPlus concatenacaoPaginao();
PaginaBPlus redistribuicaoPagina();
