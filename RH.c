#include "RH.h"

void imprimeData(Data data){
    printf("%s/%s/%s", data.dia, data.mes,data.ano);
}

void imprimeFuncionario(Funcionario funcionario){


    printf("-----------------------------------------------------\n");
    printf("| Nome: %s\n| Data de nascimento: ", funcionario.Nome);
    imprimeData(funcionario.dataNascimento);
    printf("\n| Pai: %s\n| Mãe: %s\n",);
    printf("\n| Dados Contratuais:\n");
    printf("|\t Data de Contratação: %s\n",funcionario.dadosContratuais.dataContratacao);
    printf("|\t Status de atividade: ");
    if ( funcionario.dadosContratuais.statusAtividade){
        printf("Ativo\n");
    }
    else {
        printf("Inativo\n|\tData de Desligamento: ");
        imprimeData(funcionario.dadosContratuais.dataDesligamento);
        printf("\n");
    }
    printf("-----------------------------------------------------\n");
}


int comparaChaveFuncionario(const void *a, const void *b){

    // variaveis para fazer a comparação depois
    const Chave *chaveA = (const Chave *)a;
    const Chave *chaveB = (const Chave *)b;

    // faço a comparação e volta um valor para usar nass outras funções
    int resultadoNome = strcmp(chaveA->Nome,chaveB->Nome);

    // agora eu to procurando onde esta as diferenças nas chaves
    // começo pelo proprio nome (lucas me falou q era o primeiro parametro q eu tinha q comprar)
    if (resultadoNome != 0){

        return resultadoNome;

    }

    // o segundo é a data de nascimento do funcionario, então eu vejo primeiro ano, depois mes, depois dias
    if(chaveA->dataNascimento.ano != chaveB->dataNascimento.ano){

        return ChaveA->dataNascimento.ano - chaveB->dataNascimento.ano;

    }

    if(chaveA->dataNascimento.mes != chaveB->dataNascimento.mes){

        return ChaveA->datanascimento.ano - chaveB->dataNascimento.ano;

    }

    return chaveA->dataNascimento.dia - chaveB->dataNascimento.dia;

}

void cadastrarFuncionario(FILE *arquivo, CabecalhoBPlus *cabecalho){

    // verifico se os paramentros existem para usar.
    if (!arquivo || !cabecalho){

        erro("Arquivo e cabecalho não criados corretamente na hora de cadastrar funcionario");
        return;

    }

    Funcionario funcionario;
    Funcionario existente;
    bool encontrou = false;
    long posfolha;
    int i;

    // to pegando as informações do funcionario
    printf("Digite o nome do funcionario:")
    scanf("%c", funcionario.Nome);

    printf("Digite a data de nascimento \n")
    printf("Dia: ");
    scanf("%d", &funcionario.dataNascimento.dia);

    printf("Mes: ");
    scanf("%d", &funcionario.dataNascimento.mes);

    printf("Ano: ");
    scanf("%d", &funcionario.dataNascimento.ano);

    encontrou = insereChave(arquivo, cabecalho, funcionario.chave, funcionario,
                            comparaChaveFuncionario(funcionario.chave, const void *b));

    if (encontrou){

        exito("Funcionario cadastrado com sucesso!");

    }
}

void excluirFuncionario(arquivo, cabecalho){

    Funcionario funcionario;

    printf("Digite o nome do funcionario que devera ser exlcuido:")
    scanf("%c", funcionario.Nome);

    printf("Digite a data de nascimento \n")
    printf("Dia: ");
    scanf("%d", &funcionario.dataNascimento.dia);

    printf("Mes: ");
    scanf("%d", &funcionario.dataNascimento.mes);

    printf("Ano: ");
    scanf("%d", &funcionario.dataNascimento.ano);

    encontrou = bool removeChave(arquivo, cabecalho, funcionario.chave, void comparaChaves(funcionario.chave,void *y));

    if (encontrou){

        exito("Funcionario excluido com sucesso!");

    }

}

void buscarIntervalo(FILE *arquivo, CabecalhoBPlus *cabecalho, Chave *chaveA, Chave *chaveB){
    long caminho[50];
    int tam_caminho[1] = {0};
    bool encontrou[1] = {false};

    long posPag = buscarChave(arquivo, encontrou, cabecalho, chaveA, comparaChaves, caminho, tam_caminho);

    if (!encontrou[0] || posPag == -1) {
        erro("Chave A não encontrada na árvore.\n");
        return;
    }

    while (posPag != Nulo) {
        PaginaBPlus paginaAtual = le_pagina_disco(arquivo, pos, CabecalhoBPlus *cabecalho)

        for ( int i = 0; i < paginaAtual.qtdChaves; i++) {
            void *chaveAtual = (char *)paginaAtual.chaves + (i * cabecalho->tamanhoChave);
            if (comparaChaveFuncionario(chaveAtual, chaveB) > 0) {
                free(paginaAtual.chaves);
                free(paginaAtual.ponteiros);
                return;
            }
            imprimeFuncionario((char *)paginaAtual.ponteiros + (i * cabecalho->tamanhoRegistro));
        }
        posPag = paginaAtual.proximaFolha;
        free(paginaAtual.chaves);
        free(paginaAtual.ponteiros);
    }

}
