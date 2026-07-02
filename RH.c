#include "RH.h"

void imprimeData(Data data) { printf("%d/%d/%d", data.dia, data.mes, data.ano); }

void imprimeChave(void *f) {

    Chave *chave = (Chave *)f;
    printf("[ Nome: %s | Data de nascimento: ", chave->Nome);
    imprimeData(chave->dataNascimento);
    printf(" ]");
}

void imprimeFuncionario(Funcionario funcionario) {

    // Formatação da impreção dos dados.
    printf("\n| Pai: %s\n| Mãe: %s\n", funcionario.filicao.mae, funcionario.filicao.pai);
    printf("| Dados Contratuais:\n");
    printf("|\t Data de Contratação: ");
    imprimeData(funcionario.dadosContratuais.dataContratacao);
    printf("\n");
    printf("|\t Status de atividade: ");

    // verificação de o funcionario esta ativo, se sim, imprime o status: Ativo
    if (funcionario.dadosContratuais.statusAtividade) {
        printf("Ativo\n");
    } else {
        // se não, imprime o status: Inativo, e demonstra data de desligamento.
        printf("Inativo|\tData de Desligamento: ");
        imprimeData(funcionario.dadosContratuais.dataDesligamento);
        printf("\n");
    }
    // Formatação do resto das impressoes dos dados.
    printf("| Dados de contato:\n");
    printf("|\t Endereco: %s\n", funcionario.dadosContato.endereco);
    printf("|\t Telefone: %s\n", funcionario.dadosContato.telefone);
    printf("| Historico de pagamentos: | ");
    for (int i = 0; i < 12; i++) {
        printf("%.2f |", funcionario.historicoPagamentos[i]);
    }
    printf("\n");
}
bool confereData(Data data) {
    if (data.dia < 1 || data.dia > 31 || data.mes < 1 || data.mes > 12 || data.ano > 2026) {
        return false;
    }
    if ((data.mes == 4 || data.mes == 6 || data.mes == 9 || data.mes == 11) && data.dia > 30) {
        return false;
    }
    if (data.mes == 2) {
        if ((data.ano % 4 == 0 && data.ano % 100 != 0) || (data.ano % 400 == 0)) {
            if (data.dia > 29) {
                return false;
            }
        } else {
            if (data.dia > 28) {
                return false;
            }
        }
    }
    if (data.ano == 2026 && data.mes > 7) {
        return false;
    }

    return true;
}

int comparaNome(void *a, void *b) {
    // Variaveis para fazer a comparação depois
    const Chave *chaveA = (const Chave *)a;
    const Chave *chaveB = (const Chave *)b;

    // Retorna a diferença entre o nome da chaveA e a chaveB.
    return strcmp(chaveA->Nome, chaveB->Nome);
}
int comparaChaveFuncionario(void *a, void *b) {

    int resultadoNome = comparaNome(a, b);

    // verifica se os nomes são iguais
    if (resultadoNome != 0) {

        return resultadoNome;
    }

    Chave *chaveA = (Chave *)a;
    Chave *chaveB = (Chave *)b;

    // verifica se o ano de nascimento dos dois funcionarios são diferentes, se
    // sim, retorna a diferença entre os dois.
    if (chaveA->dataNascimento.ano != chaveB->dataNascimento.ano) {

        return chaveA->dataNascimento.ano - chaveB->dataNascimento.ano;
    }

    // verifica se o mes de nascimento dos dois funcionarios são diferentes, se
    // sim, retorna a diferença entre os dois.
    if (chaveA->dataNascimento.mes != chaveB->dataNascimento.mes) {

        return chaveA->dataNascimento.mes - chaveB->dataNascimento.mes;
    }

    // retorna a diferença entre os dias de nascença.
    return chaveA->dataNascimento.dia - chaveB->dataNascimento.dia;
}

void cadastrarFuncionario(FILE *arquivo, CabecalhoBPlus *cabecalho) {

    // verifica a existencia dos parametros.
    if (!arquivo || !cabecalho) {
        // se utiliza da função de erro para formatar a mensagem apresentando erro
        erro("Arquivo e cabecalho não criados corretamente na hora de cadastrar "
             "funcionario");
        return;
    }

    Funcionario funcionario;
    Funcionario existente;
    bool encontrou = false;
    long posfolha;
    int i;

    // Recebe as informações do funcionario que sera cadastrado
    printf("Digite o nome do funcionario:");
    fgets(funcionario.Nome, RH_TAM_NOME, stdin);
    funcionario.Nome[strcspn(funcionario.Nome, "\n")] = '\0';

    // recebe os dados cadastrais do funcionario;
    while (1) {
        printf("Digite a data de nascimento do funcionário (dd/mm/aaaa): ");
        scanf("%d/%d/%d", &funcionario.dataNascimento.dia, &funcionario.dataNascimento.mes,
              &funcionario.dataNascimento.ano);
        bool dataValida = confereData(funcionario.dataNascimento);

        if (dataValida) {
            break; // Sai do loop se a data for válida
        } else {
            erro("Data inválida. Por favor, insira novamente");
        }
    }

    // utilizado para limpar o buffer
    while (getchar() != '\n')
        ;

    printf("Digite o nome do pai do funcionario:");
    fgets(funcionario.filicao.pai, RH_TAM_NOME, stdin);
    funcionario.filicao.pai[strcspn(funcionario.filicao.pai, "\n")] = '\0';

    printf("Digite o nome da mae do funcionario:");
    fgets(funcionario.filicao.mae, RH_TAM_NOME, stdin);
    funcionario.filicao.mae[strcspn(funcionario.filicao.mae, "\n")] = '\0';

    printf("Digite o endereco do funcionario:");
    fgets(funcionario.dadosContato.endereco, 150, stdin);
    funcionario.dadosContato.endereco[strcspn(funcionario.dadosContato.endereco, "\n")] = '\0';

    printf("Digite o telefone do funcionario:");
    fgets(funcionario.dadosContato.telefone, 20, stdin);
    funcionario.dadosContato.telefone[strcspn(funcionario.dadosContato.telefone, "\n")] = '\0';

    printf("Digite a data de contratacao do funcionario \n");

    while (1) {
        printf("Digite a data de contratacao do funcionário (dd/mm/aaaa): ");
        scanf("%d/%d/%d", &funcionario.dadosContratuais.dataContratacao.dia,
              &funcionario.dadosContratuais.dataContratacao.mes,
              &funcionario.dadosContratuais.dataContratacao.ano);

        bool dataValida = confereData(funcionario.dadosContratuais.dataContratacao);

        if (dataValida) {
            break; // Sai do loop se a data for válida
        } else {
            erro("Data inválida. Por favor, insira novamente");
        }
    }

    printf("Digite o status de atividade do funcionario (1 para ativo, 0 para inativo): ");
    int aux = 0;
    scanf("%d", &aux);

    funcionario.dadosContratuais.statusAtividade = aux > 0 ? true : false;

    if (funcionario.dadosContratuais.statusAtividade == 0) {
        printf("Digite a data de desligamento do funcionário (dd/mm/aaaa): ");
        scanf("%d/%d/%d", &funcionario.dadosContratuais.dataDesligamento.dia,
              &funcionario.dadosContratuais.dataDesligamento.mes,
              &funcionario.dadosContratuais.dataDesligamento.ano);
    } else {
        funcionario.dadosContratuais.dataDesligamento.dia = 0;
        funcionario.dadosContratuais.dataDesligamento.mes = 0;
        funcionario.dadosContratuais.dataDesligamento.ano = 0;
    }
    printf("Digite o historico de pagamentos do funcionario (12 meses):\n");
    for (i = 0; i < 12; i++) {
        printf("Mes %d: ", i + 1);
        scanf("%f", &funcionario.historicoPagamentos[i]);
    }
    // utilizado para limpar o buffer
    while (getchar() != '\n')
        ;

    // se utiliza da função insereChave para inserir e verificar a possibilidade
    // de inserção, retornado TRUE, se foi bem sucedida, e FALSE, se mal sucedida
    Chave chaveFuncionario;
    strcpy(chaveFuncionario.Nome, funcionario.Nome);
    chaveFuncionario.dataNascimento.dia = funcionario.dataNascimento.dia;
    chaveFuncionario.dataNascimento.mes = funcionario.dataNascimento.mes;
    chaveFuncionario.dataNascimento.ano = funcionario.dataNascimento.ano;
    encontrou =
        insereChave(arquivo, cabecalho, &chaveFuncionario, &funcionario, comparaChaveFuncionario);

    // retorna uma mensagem de exito, se utilizando da função exito, para exibir
    // a mensagem de sucesso ao usuario.
    if (encontrou) {
        exito("Funcionario cadastrado com sucesso!");
    } else {
        erro("Não foi possível cadastrar.");
    }
}

void excluirFuncionario(FILE *arquivo, CabecalhoBPlus *cabecalho) {

    Funcionario funcionario;

    // recebe as informações do funcionario para a exclusão.
    printf("Digite o nome do funcionario que devera ser exlcuido:");
    fgets(funcionario.Nome, RH_TAM_NOME, stdin);
    funcionario.Nome[strcspn(funcionario.Nome, "\n")] = '\0';

    printf("Digite a data de nascimento \n");
    while (1) {
        printf("Digite a data de nascimento do funcionário (dd/mm/aaaa): ");
        scanf("%d/%d/%d", &funcionario.dataNascimento.dia, &funcionario.dataNascimento.mes,
              &funcionario.dataNascimento.ano);
        bool dataValida = confereData(funcionario.dataNascimento);

        if (dataValida) {
            break; // Sai do loop se a data for válida
        } else {
            erro("Data inválida. Por favor, insira novamente");
        }
    }

    // inicialização das variaveis que serão utilizadas posteriormente.
    Chave chaveFuncionario;
    strcpy(chaveFuncionario.Nome, funcionario.Nome);
    chaveFuncionario.dataNascimento.dia = funcionario.dataNascimento.dia;
    chaveFuncionario.dataNascimento.mes = funcionario.dataNascimento.mes;
    chaveFuncionario.dataNascimento.ano = funcionario.dataNascimento.ano;

    // utiliza a função removeChave para remover e verificar a possibilidade de remoção do usuario
    // retornando um valor booleano de acordo com o resultado.
    bool encontrou = removeChave(arquivo, cabecalho, &chaveFuncionario, comparaChaveFuncionario);

    if (encontrou) {

        exito("Funcionario excluido com sucesso!");
    }
}

int auxBusca(FILE *arquivo, CabecalhoBPlus *cabecalho, Chave *chaveA, Chave *chaveB,
             bool impressaoCompleta, int compara(void *a, void *b)) {
    long caminho[50];
    int tam_caminho        = 0;
    int qtdChavesImpressas = 0;
    bool encontrou         = false;
    long posPag            = Nulo;

    // utiliza a função buscarChave para encontrar a posição da pagina desejada
    posPag = buscarChave(arquivo, &encontrou, cabecalho, chaveA, compara, caminho, &tam_caminho);

    // verifica se a chave foi encontrada, se não, retorna um aviso ao usuario.
    if (posPag == Nulo || posPag == -1) {
        erro("Árvore vazia ou erro na busca.\n");
        return 0;
    }
    if (!encontrou || posPag == -1) {
        erro("Chave não encontrada na árvore.\n");
        return 0;
    }
    Funcionario funcionarioAtual;

    // percorre as folhas da arvoreB+.
    while (posPag != Nulo) {

        // lê o disco se utilizando da função le_pagina_disco.
        PaginaBPlus paginaAtual = le_pagina_disco(arquivo, posPag, cabecalho);

        // percorre todas as chaves da folha atual
        for (int i = 0; i < paginaAtual.qtdChaves; i++) {

            // calcula o endereço da chave que esta na posição i da folha
            void *chaveAtual = (char *)paginaAtual.chaves + (i * cabecalho->tamanhoChave);

            // se a chave atual extrapolou o limite, parar a leitura.
            if (compara(chaveAtual, chaveB) > 0) {
                posPag = Nulo;
                break;
            }
            if ( compara(chaveAtual, chaveA) < 0) {
                continue;
            }

            // Guarda local atual em funcionarioAtual.
            funcionarioAtual =
                *(Funcionario *)((char *)paginaAtual.ponteiros + (i * cabecalho->tamanhoRegistro));

            // imprime os dados dos funcionarios.
            if (impressaoCompleta) {
                imprimeChave(&funcionarioAtual);
                imprimeFuncionario(funcionarioAtual);
            } else {
                imprimeChave(&funcionarioAtual);
            }

            qtdChavesImpressas++;
        }
        if (posPag != Nulo) {
            posPag = paginaAtual.proximaFolha;
        }

        free(paginaAtual.chaves);
        free(paginaAtual.ponteiros);
    }
    if (!impressaoCompleta) {
        if (qtdChavesImpressas == 1) {
            imprimeFuncionario(funcionarioAtual);
        }
        else {
            printf("\n");
        }
    }
    return qtdChavesImpressas;
}

void buscaIntervalo(FILE *arquivo, CabecalhoBPlus *cabecalho) {

    // verifica se os parametros são validos
    if (!arquivo || !cabecalho)
        erro("Parametros invalidos passados para buscaIntervalo");

    Chave chaveA, chaveB;

    printf("Digite o nome do funcionario que deseja buscar (inicio do intervalo): ");
    fgets(chaveA.Nome, RH_TAM_NOME, stdin);
    chaveA.Nome[strcspn(chaveA.Nome, "\n")] = '\0';
    printf("Digite o nome do funcionario que deseja buscar (fim do intervalo): ");
    fgets(chaveB.Nome, RH_TAM_NOME, stdin);
    chaveB.Nome[strcspn(chaveB.Nome, "\n")] = '\0';
    auxBusca(arquivo, cabecalho, &chaveA, &chaveB, true, comparaNome);
}

void buscaFuncionario(FILE *arquivo, CabecalhoBPlus *cabecalho) {

    // verifica se os parametros são validos
    if (!arquivo || !cabecalho)
        erro("Parametros invalidos passados para buscaFuncionario");

    Chave chaveA;
    printf("Digite o nome do funcionario que deseja buscar: ");
    fgets(chaveA.Nome, RH_TAM_NOME, stdin);
    chaveA.Nome[strcspn(chaveA.Nome, "\n")] = '\0';
    chaveA.dataNascimento.dia               = 0;
    chaveA.dataNascimento.mes               = 0;
    chaveA.dataNascimento.ano               = 0;
    printf("\n");
    int qtdChaves = auxBusca(arquivo, cabecalho, &chaveA, &chaveA, false, comparaNome);

    if (qtdChaves == 0) {
        printf("Nenhum funcionário encontrado com o nome '%s'.\n", chaveA.Nome);
    } else {
        if (qtdChaves == 1) {
            return;
        } else {
            printf("Foram encontrados %d funcionários com o nome '%s'.\n", qtdChaves, chaveA.Nome);
            printf("Digite a data de aniversário do funcionário que deseja buscar "
                   "(dd/mm/aaaa): ");
            scanf("%d/%d/%d", &chaveA.dataNascimento.dia, &chaveA.dataNascimento.mes,
                  &chaveA.dataNascimento.ano);
            auxBusca(arquivo, cabecalho, &chaveA, &chaveA, true, comparaChaveFuncionario);
        }
    }
}
