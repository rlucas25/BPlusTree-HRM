#ifndef RH_H
#define RH_H

#include <stdio.h>
#include <stdbool.h>
#include "Bplus.h"

#define RH_TAM_NOME       80
#define RH_TAM_ENDERECO   120
#define RH_TAM_TELEFONE   20
#define RH_MESES_HIST     12
#define RH_MAX_HOMONIMOS  100

typedef struct {

    int dia;
    int mes;
    int ano;

} DataRH;

typedef struct {

    char nome  [RH_TAM_NOME];
    DataRH nascimento;

}ChaveFuncionario;

typedef enum{

    FUNCIONARIO_INATIVO = 0;
    FUNCIONARIO_ATIVO = 1;

}StatusFuncionario;

typedef struct{

    int mes;
    int ano;
    double salarioBruto;
    double descontos;
    double salarioLiquido;
    bool pago;

}pagamentoRH;

typedegf struct{

    ChaveFuncionario chave;
    char nomeMae[RH_TAM_NOME];
    char nomePai[RH_TAM_NOME];

    char endereco[RH_TAM_ENDERECO];
    char telefone[RH_TAM_TELEFONE];

    DataRH dataContratacao;
    StatusFuncionario status;
    DataRH dataDesligamento;

    pagamentoRH historico[RH_MESES_HIST];
    int qtdPagamentos;

}Funcionario;

int comparaChaveFuncionario(const void *a, const void *b);
void cadastrarFuncionario(FILE *arquivo, CabecalhoBPlus *cabecalho);
void buscarFuncionario(FILE *arquivo, CabecalhoBPlus *cabecalho);
void excluirFuncionario(FILE *arquivo, CabecalhoBPlus *cabecalho);
void listarFuncionariosIntervalo(FILE *arquivo, CabecalhoBPlus *cabecalho);
void menuRH(FILE *arquivo, CabecalhoBPlus *cabecalho);