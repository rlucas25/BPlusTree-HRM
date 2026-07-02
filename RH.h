#ifndef RH_H
#define RH_H
#include "Bplus.h"

#define RH_TAM_NOME 100
#define NOME_ARQUIVO "funcionarios.rh"
#define ORDEM_P 4

// struct usada para manipular as datas dos funcionarios, dividido em
// dia/mes/ano.
typedef struct {
    int dia;
    int mes;
    int ano;
} Data;

// struct que representa a filiação (nome do pai e da mãe) do funcionario.
typedef struct {
    char pai[RH_TAM_NOME];
    char mae[RH_TAM_NOME];
} Filiacao;

// struct utilizada para armazenar e manipular os contatos (endereço e telefone)
// do funcionario.
typedef struct {
    char endereco[150];
    char telefone[20];
} DadosContato;

// struct utilizada para armazenar e manipular dados contratuais do funcionario.
typedef struct {
    Data dataContratacao;
    bool statusAtividade;
    Data dataDesligamento;
} DadosContratuais;

// struct auxiliar utilizada para facilitar manipulçao das chaves (nome e data
// de nascimento).
typedef struct {
    char Nome[RH_TAM_NOME];
    Data dataNascimento;
} Chave;

// struct que se utiliza das structs anteriores para reunir as informações, e
// tambem guardar o nome e o historico de pagamento do usuario.
typedef struct {
    char Nome[RH_TAM_NOME];
    Data dataNascimento;
    Filiacao filicao;
    DadosContato dadosContato;
    DadosContratuais dadosContratuais;
    float historicoPagamentos[12];
} Funcionario;

// Realiza a impressão de uma data no formato dd/mm/aaaa
void imprimeData(Data data);

// Imprime o nome e a data de nascimento do funcionário
void imprimeChave(void *f);

// Imprime todas as informações do funcionário
void imprimeFuncionario(Funcionario funcionario);

// Compara dois funcionários com base no nome
int comparaNome(void *a, void *b);

// Compara dois funcionario com base no nome e data de nascimento
int comparaChaveFuncionario(void *a, void *b);

// cadastra funcionario com dados inserido pelo usuario, tambem verificando se
// ja existe
// algum funcionario com aqueles dados, se sim, exibe a mensagem ao usuario.
void cadastrarFuncionario(FILE *arquivo, CabecalhoBPlus *cabecalho);

// Exclui um funcionario da estrutura
void excluirFuncionario(FILE *arquivo, CabecalhoBPlus *cabecalho);

// Lê o nome do funcionarios e busca os funcionários dentro deste intervalo de
// nomes,
//  por fim imprime as informações dos funcionários encontrados
void buscaIntervalo(FILE *arquivo, CabecalhoBPlus *cabecalho);

// Busca um funcionário específico com base no nome
void buscaFuncionario(FILE *arquivo, CabecalhoBPlus *cabecalho);

#endif