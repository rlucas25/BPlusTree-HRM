// Turma: 3704.1
// Grupo 14
// DENNIS FRANCISCO GUIMARÃES DE OLIVEIRA BARACHO; 2024200496
// HEITOR SANTOS MAIA; 2024200498
// LUCAS REBOUÇAS ALMEIDA; 2024200501
// MARCOS VINÍCIUS PIMENTEL GOMES; 2024200500

#ifndef RH_H
#define RH_H
#include "Bplus.h"

#define RH_TAM_NOME 100
#define NOME_ARQUIVO "funcionarios.rh"
#define ORDEM_P 4


/**
 * @brief Estrutura que representa a data de nascimento do funcionario
 * 
 */
typedef struct {
    int dia;    /* Valor inteiro que representa o dia da data de nascimento */
    int mes;    /* Valor inteiro que representa o mes da data de nascimento */
    int ano;    /* Valor inteiro que representa o ano da data de nascimento */
} Data;


/**
 * @brief Estrutura que representa a filiação do funcionario
 * 
 */
typedef struct {
    char pai[RH_TAM_NOME];  /* Nome do pai do funcionario */
    char mae[RH_TAM_NOME];  /* Nome da mãe do funcionario */
} Filiacao;


/**
 * @brief Estrutura que representa os dados de contato do funcionario
 * 
 */
typedef struct {
    char endereco[150]; /* Valor do tipo char que representa Endereço do funcionario */
    char telefone[20];  /* Valor do tipo char que Telefone do funcionario */
} DadosContato;


/**
 * @brief Estrutura que representa os dados contratuais do funcionario
 * 
 */
typedef struct {
    Data dataContratacao;   /* Data da contratação do usuario */
    bool statusAtividade;   /* Status do funcionario, sendo "Ativo" ou "Inativo" */
    Data dataDesligamento;  /* Data do desligamento do funcionario, se houver */
} DadosContratuais;


/**
 * @brief Estrutura que representa os dados principais do funcionario.
 * 
 */
typedef struct {
    char Nome[RH_TAM_NOME]; /* Nome do funcionario */
    Data dataNascimento;    /* Data de nascimento do funcionario */
} Chave;


/**
 * @brief Estrutura que representa todos os dados do funcionario
 * 
 */
typedef struct {
    char Nome[RH_TAM_NOME]; /* Nome do funcionario */
    Data dataNascimento;    /* Data de nascimento do funcionario */
    Filiacao filicao;   /* Filiação do funcionario */
    DadosContato dadosContato;  /* Dados de contato do funcionario */
    DadosContratuais dadosContratuais;  /* Dados Contratuais do funcionario */
    float historicoPagamentos[12];  /* Vetor com os valores do pagamento de cada mes */
} Funcionario;


/**
 * @brief Imprime os valores da data de nascimento do funcionario.
 * 
 * @details A impressão é feita da forma dd/mm/aaaa
 * @param data os valores do tipo Data que representam a data de nascimento do funcionario.
 */
void imprimeData(Data data);


/**
 * @brief Imprime os valores principais, sendo eles o nome e a data de nascimento do funcionario
 *
 * @details A impessão é feita da forma [Nome: xxxx | Data de Nascimento: dd/mm/aaaa]
 * @param f ponteiro para a variavel que representa o funcionario
 */
void imprimeChave(void *f);


/**
 * @brief Imprime apenas o primeiro nome do funcionario
 * 
 * @details A impressão é feita da forma [Nome: xxxx | Data de nascimento: dd/mm/aaaa]
 * @param f ponteiro para variavel que representa funcionario
 */
void imprimeChavePrimeiroNome(void *f);


/**
 * @brief Imprime todas as informções do funcionario
 * 
 * @details A impressão é feita de forma que facilite a visualização do usuario
 * @param f ponteiro para variavel que representa funcionario
 */
void imprimeFuncionario(void *f);


/**
 * @brief Compara os nomes de dois funcionarios e retorna a diferença
 * 
 * @param a ponteiro para a variavel do primeiro funcionario
 * @param b ponteiro para a variavel do segundo funcionario
 * @return int diferença entre os nomes
 */
int comparaNome(void *a, void *b);


/**
 * @brief Compara o nome e a data de nascimento de dois funcionarios e retonra a diferença
 * 
 * @param a ponteiro para a variavel do primeiro funcionario
 * @param b ponteiro para a variavel do segundo funcionario
 * @return int diferença entre as chaves
 */
int comparaChaveFuncionario(void *a, void *b);


/**
 * @brief Verifica se a data de nascimento do funcionario é valida
 * 
 * @param data data de nascimento do usuario
 * @return bool se a data for valida ou não
 */
bool confereData(Data data);


/**
 * @brief cadastra novo funcionario no sistema
 * 
 * @param arquivo arquivo que armazena as informações do funcionario
 * @param cabecalho cabeçalho que guarda as infromações sobre a arvore
 */
void cadastrarFuncionario(FILE *arquivo, CabecalhoBPlus *cabecalho);


/**
 * @brief Exclui funcionario do sistema
 * 
 * @param arquivo arquivo que armazena as informações do funcionario
 * @param cabecalho cabeçalho que guarda as infromações sobre a arvore
 */
void excluirFuncionario(FILE *arquivo, CabecalhoBPlus *cabecalho);


/**
 * @brief busca funcionario dentro de um intervalo requerido
 * 
 * @param arquivo arquivo que armazena as informações do funcionario
 * @param cabecalho cabeçalho que guarda as infromações sobre a arvore
 */
void buscaIntervaloFuncionario(FILE *arquivo, CabecalhoBPlus *cabecalho);


/**
 * @brief busca um funcionario na arvore armazenada no disco
 * 
 * @param arquivo arquivo que armazena as informações do funcionario
 * @param cabecalho cabeçalho que guarda as infromações sobre a arvore
 */
void buscaFuncionario(FILE *arquivo, CabecalhoBPlus *cabecalho);


/**
 * @brief Imprime a arvore demonstrando niveis, primeiro nome e data de cada funcionario
 * 
 * @param arquivo arquivo que armazena as informações do funcionario
 * @param cabecalho cabeçalho que guarda as infromações sobre a arvore
 */
void imprimeArvoreFuncionario(FILE *arquivo, CabecalhoBPlus *cabecalho);

#endif