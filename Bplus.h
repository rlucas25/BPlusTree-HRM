#ifndef Bplus_H
#define Bplus_H

#include "Util.h"
#include <string.h>

#define Nulo -1

/**
 * @brief Estrutura que representa os dados referentes a arvore B+ armazenada no disco
 * 
 */
typedef struct {
    
    long posRaiz;   /* Valor do tipo long que representa a posição da raiz no disco rigido */
    long ordemP;    /* Valor do tipo long que representa a ordem  de ponteiro adotada na arvore */
    size_t tamanhoChave;    /* Valor do tipo size_t que representa o tamanho das chaves armazenadas na arvore */
    size_t tamanhoRegistro; /* Valor do tipo size_t que representa o tamanho total da estrutura funcionario */
    long proxNolivre;   /* Valor do tipo long que representa o proximo local licre no disco */
} CabecalhoBPlus;


/**
 * @brief Estrutura que representa as informções das paginas e valores da arvore B+ armezanada no disco
 * 
 */
typedef struct {

    int qtdChaves;  /* Valor inteiro que representa a quantidade de chaves existentes na pagina */
    void *chaves;   /* ponteiro que representa as chaves do funcionario  */
    void *ponteiros;    /* ponteiro que representa os ponteiros para paginas filhas */
    bool ehfolha;   /* Valor booleano que representa se a pagina é folha ou não */
    long proximaFolha;  /* Valor do tipo long que representa a posição da proxima pagina folha */
} PaginaBPlus;


/**
 * @brief realiza o calculo do maximo de chaves que pode ter em uma pagina
 * 
 * @param cabecalho cabeçalho que guarda as infromações sobre a arvore
 * @param ehfolha variavel booleana que representa estado de ser folha ou não
 * @return int valor de chave maxima aceita na pagina
 */
int maxChavesPagina(CabecalhoBPlus *cabecalho, bool ehfolha);


/**
 * @brief Realiza o calculo para encontrar a metade da (ordem +1)
 * 
 * @param cabecalho cabeçalho que guarda as infromações sobre a arvore
 * @return int resultado do calculo referente a metade da (ordem +1)
 */
int metadeArredondada(CabecalhoBPlus *cabecalho);


/**
 * @brief Encontra o valor que representa o minimo de chaves que tem de haver nas paginas
 * 
 * @param cabecalho cabeçalho que guarda as infromações sobre a arvore
 * @param ehfolha variavel booleana que representa estado de ser folha ou não
 * @return int valor minimo de chaves na pagina
 */
int minChavesPagina(CabecalhoBPlus *cabecalho, bool ehfolha);


/**
 * @brief cria a arvore e inicializa as variaveis essenciais
 * 
 * @param nomeArquivo nome do arquivo que sera criado
 * @param ordem valor que rege o funcionamento da arvore
 * @param tamanhoChave valor que representa o tamanho das chaves
 * @param tamanhoRegistro valor que representa tamanho total da estrutura funcionario
 * @return bool status que representa se a arvore foi criada com sucesso ou não 
 */
bool criaArvore(const char *nomeArquivo, int ordem, size_t tamanhoChave, size_t tamanhoRegistro);


/**
 * @brief Abre a arvore
 * 
 * @param nomeArquivo nome do arquivo que sera criado
 * @param cabecalho cabeçalho que guarda as infromações sobre a arvore.
 * @return arquivo que foi aberto
 */
FILE *abrirArvore(const char *nomeArquivo, CabecalhoBPlus *cabecalho);


/**
 * @brief Le a pagina diretamente do disco rigido e retornaoffseta pagina para memoria RAM
 * 
 * @param arquivo variavel que armazena dados dos funcionarios
 * @param offset variavel que representa o local onde o ponteiro esta no disco rigido
 * @param cabecalho cabeçalho que guarda as infromações sobre a arvore.
 * @return PaginaBPuls pagina recuperada do disco rigido
 */
PaginaBPlus le_pagina_disco(FILE *arquivo, long offset, CabecalhoBPlus *cabecalho);


/**
 * @brief Fecha a arvore aberta na memoria RAM
 * 
 * @param arquivoAberto ponteiro para o arquivo aberto na memoria RAM
 */
void fecharArvore(FILE *arquivoAberto);


/**
 * @brief Busca a chave na arvore
 * 
 * @param arquivoAberto ponteiro para o arquivo aberto
 * @param encontrou variavel que representa se a chave foi encontrada ou não
 * @param cabecalho ponteiro para cabeçalho que guarda as infromações sobre a arvore.
 * @param chave ponteiro para chave que esta sendo buscada
 * @param comparaChaves ponteiro para função que ira comparar as chaves
 * @param caminho   ponteiro para variavel que armazena o caminho percorrida
 * @param tam_caminho ponteiro para variavel que armazena o tamanho do caminho
 * @return long posição no disco da chave buscada
 */
long buscarChave(FILE *arquivoAberto, bool *encontrou, CabecalhoBPlus *cabecalho, void *chave,
                 int (*comparaChaves)(void *x, void *y), long *caminho, int *tam_caminho);


/**
 * @brief Realiza a cisão em uma pagina que ultrapassou o limite
 * 
 * @param pagina ponteiro para pagina onde sera realizada a cisão
 * @param cabecalho ponteiro para cabeçalho que guarda as infromações sobre a arvore.
 * @param chavePromovida ponteiro para chave que sera movimentada para o pai
 * @return PaginaBplus retorna a nova pagina criada depois da cisão
 */
PaginaBPlus *cisaoPagina(PaginaBPlus *pagina, CabecalhoBPlus *cabecalho,
                         void *chavePromovida);

/**
 * @brief insere uma nova chave na arvore
 * 
 * @param arquivoAberto ponteiro para o arquivo aberto no momento
 * @param cabecalho ponteiro para cabeçalho que guarda as infromações sobre a arvore.
 * @param chave ponteiro para a chave que esta sendo inserida
 * @param registro ponteiro para o registro
 * @param comparaChaves ponteiro para a função que compara as chaves
 * @return bool status que representa se a inserção foi realizada com sucesso ou não
 */                 
bool insereChave(FILE *arquivoAberto, CabecalhoBPlus *cabecalho, void *chave, const void *registro,
                 int (*comparaChaves)(void *x, void *y));


/**
 * @brief Aloca a pagina da arvore
 * 
 * @param arquivoAberto ponteiro para o arquivo aberto no momento
 * @param cabecalho ponteiro para cabeçalho que guarda as infromações sobre a arvore.
 * @return long local da pagina alocada
 */
long alocarPagina(FILE *arquivoAberto, CabecalhoBPlus *cabecalho);


/**
 * @brief remove a chave da arvore
 * 
 * @param arquivoAbertoponteiro ponteiro para o arquivo aberto no momento
 * @param cabecalho ponteiro para cabeçalho que guarda as infromações sobre a arvore.
 * @param chave ponteiro para a chave que esta sendo removida
 * @param comparaChaves ponteiro para a função que compara as chaves
 * @return bool status que representa se a remoção foi realizada com sucesso ou não
 */
bool removeChave(FILE *arquivoAberto, CabecalhoBPlus *cabecalho, void *chave,
                 int (*comparaChaves)(void *x, void *y));


/**
 * @brief Aloca uma imagem com as dimensões especificadas.
 * 
 * @param arquivo ponteiro para variavel que armazena dados dos funcionarios
 * @param atual ponteiro para a pagina atual
 * @param irmao ponteiro para a pagina irmão
 * @param chavePai ponteiro para a pagina pai
 * @param cabecalho ponteiro para cabeçalho que guarda as infromações sobre a arvore.
 */
void concatenacao(FILE *arquivo, PaginaBPlus *atual, PaginaBPlus *irmao, const void *chavePai,
                  CabecalhoBPlus *cabecalho);


/**
 * @brief Realiza a redistribuição entre paginas
 * 
 * @param atual ponteiro para a pagina atual
 * @param irmao ponteiro para a pagina irmão
 * @param ehEsq variavel que representa se a pagina esta a esquerda ou não
 * @param idxFilhoAtual inteiro que representa posição na pagina
 * @param pai ponteiro para pagina pai
 * @param cabecalho ponteiro para cabecaho que guarda as infromações sobre a arvore.
 * @return PaginaBPlus pagina que sofreu redistribuição
 */
PaginaBPlus redistribuicaoPagina(PaginaBPlus *atual, PaginaBPlus *irmao, bool ehEsq, int idxFilhoAtual,
                    PaginaBPlus *pai, CabecalhoBPlus *cabecalho);


/**
 * @brief Balancea a arvore de baixo para cima
 * 
 * @param arquivoAberto ponteiro para o arquivo aberto no momento
 * @param cabecalho ponteiro para cabecaho que guarda as infromações sobre a arvore.
 * @param posNosAtual posição para a chave atual
 * @param caminho ponteiro para variavel que armazena o caminho percorrida
 * @param tam_caminho ponteiro para variavel que armazena o tamanho do caminho
 */
void BalancearArvoreUnderflow(FILE *arquivoAberto, CabecalhoBPlus *cabecalho, long posNoAtual,
                              long *caminho, int tam_caminho);


/**
 * @brief imprime a pagina
 * 
 * @param pagina variavel que armazena a pagina que sera inserida
 * @param cabecalho ponteiro para cabecaho que guarda as infromações sobre a arvore.
 * @param imprimeChave ponteiro para função que ira imprimir a chave
 */                              
void imprimePagina(PaginaBPlus pagina, CabecalhoBPlus *cabecalho,
                   void (*imprimechave)(void *chave));


/**
 * @brief Imprime a arvore BPlus
 * 
 * @param arquivo ponteiro para variavel que armazena dados dos funcionarios
 * @param cabecalho ponteiro para cabecaho que guarda as infromações sobre a arvore.
 * @param posisaoPagina variavel que armazena a posiçao da pagina
 * @param prefixo ponteiro para a variavel que ira armazenar o prefixo que sera usado ao imprimir a arvore
 * @param ehUltimo variavel booleana que representa o estado de ser o ultimo ou não
 * @param imprimeNo ponteiro para função que ira imprimir a chave
 */                  
void imprimeBPlus(FILE *arquivo, CabecalhoBPlus *cabecalho, long posisaoPagina, const char *prefixo,
                  bool ehUltimo, void (*imprimeNo)(void *chave));



/**
 * @brief Aloca uma imagem com as dimensões especificadas.
 * 
 * @param arquivo ponteiro para variavel que armazena dados dos funcionarios
 * @param cabecalho ponteiro para cabecaho que guarda as infromações sobre a arvore.
 * @param chaveA ponteiro para variavel que representa a primeira chave do intervalo
 * @param chaveB ponteiro para variavel que representa a segunda chave do intervalo
 * @param compara ponteiro para função que ira comparar as chaves
 * @param imprime ponteiro para a função que impime oque for nescessario
 * @param ehChave variavel booleana para verificar se é a chave ou não
 * @param registroEncotrado ponteiro para variavel que recebe o ultimo registro
 * @return int quantidade de chaves que foi encontrada
 */                  
int buscarPorIntervalo(FILE *arquivo, CabecalhoBPlus *cabecalho, void *chaveA, void *chaveB,
                       int compara(void *a, void *b), void imprime(void *a), bool ehChave,
                       void *registroEncontrado);
#endif