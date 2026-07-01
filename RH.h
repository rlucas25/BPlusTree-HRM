#ifndef RH_H
#define RH_H

#include <stdio.h>
#include <stdbool.h>
#include "Bplus.h"

#define RH_TAM_NOME 100

typedef struct {
    int dia;
    int mes;
    int ano;

}Data;

typedef struct {
    char pai[RH_TAM_NOME];
    char mae[RH_TAM_NOME];


}Filiacao;

typedef struct {
    char endereco[150];
    int char[20];
}DadosContato;

typedef struct {

    Data dataContratacao;
    bool statusAtividade;
    Data dataDesligamento;

}DadosContratuais;

typedef struct {

    char Nome[RH_TAM_NOME];
    Data dataNascimento;
    Filiacao filicao;
    DadosContato dadosContato;
    DadosContratuais dadosContratuais;
    float historicoPagamentos[12];

}Funcionario;


typedef struct {
    char Nome[RH_TAM_NOME];
    Data dataNascimento;
}Chave;

void imprimeData(Data data);
int comparaChaveFuncionario(const void *a, const void *b);
void cadastrarFuncionario(FILE *arquivo, CabecalhoBPlus *cabecalho);
void excluirFuncionario(FILE *arquivo, CabecalhoBPlus *cabecalho);
