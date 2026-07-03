#include "RH.h"
#include "Bplus.h"
#include <stdio.h>

void imprimeData(Data data) { printf("%d/%d/%d", data.dia, data.mes, data.ano); }

void imprimeChave(void *f) {

    // impressão das chaves de determinado funcionario
    Chave *chave = (Chave *)f;
    printf("[Nome: %s | Data de nascimento: ", chave->Nome);
    imprimeData(chave->dataNascimento);
    printf("]");
}
void imprimeChavePrimeiroNome(void *f) {
    // impressão das chaves de determinado funcionario
    Chave *chave = (Chave *)f;

    int tamanho = strcspn(chave->Nome, " ");

    printf("[%.*s | ", tamanho, chave->Nome);
    imprimeData(chave->dataNascimento);
    printf("]");
}

void imprimeFuncionario(void *f) {
    Funcionario funcionario = *(Funcionario *)f;

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

    // verifica se as informações inseridas estçao corretas em relação a data atual
    if (data.dia < 1 || data.dia > 31 || data.mes < 1 || data.mes > 12 || data.ano > 2026) {
        return false;
    }
    // verifica se as informações inseridas estçao corretas em relação ao dia do mes
    if ((data.mes == 4 || data.mes == 6 || data.mes == 9 || data.mes == 11) && data.dia > 30) {
        return false;
    }

    // trata o caso do mes de fevereiro e ano bissexto.
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

    // não permite que esteja meses no futudo
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
        int lidos = scanf("%d/%d/%d", &funcionario.dataNascimento.dia,
                          &funcionario.dataNascimento.mes, &funcionario.dataNascimento.ano);
        // limpar buffer
        while (getchar() != '\n')
            ;

        if (lidos == 3) {
            bool dataValida = confereData(funcionario.dataNascimento);
            if (dataValida) {
                // Sai do loop se o formato e a data forem válidos
                break;
            } else {
                erro("Data inválida. Por favor, insira novamente.");
            }
        } else {
            erro("Formato incorreto. Utilize apenas números no formato dd/mm/aaaa.");
        }
    }

    // recebe informções do usuario
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
        int lidos = scanf("%d/%d/%d", &funcionario.dadosContratuais.dataContratacao.dia,
                          &funcionario.dadosContratuais.dataContratacao.mes,
                          &funcionario.dadosContratuais.dataContratacao.ano);
        while (getchar() != '\n')
            ;

        if (lidos == 3) {
            bool dataValida = confereData(funcionario.dataNascimento);
            if (dataValida) {
                // Sai do loop se o formato e a data forem válidos
                break;
            } else {
                erro("Data inválida. Por favor, insira novamente.");
            }
        } else {
            erro("Formato incorreto. Utilize apenas números no formato dd/mm/aaaa.");
        }
    }

    int aux = -1;
    while (1) {
        printf("Digite o status de atividade do funcionario (1 para ativo, 0 para inativo): ");
        int lidos = scanf("%d", &aux);

        // Limpa o buffer
        while (getchar() != '\n')
            ;

        // Só aceita se leu 1 número com sucesso E se esse número for 0 ou 1
        if (lidos == 1 && (aux == 0 || aux == 1)) {
            break;
        } else {
            erro("Entrada inválida. Digite exclusivamente 1 (ativo) ou 0 (inativo).");
        }
    }

    funcionario.dadosContratuais.statusAtividade = aux > 0 ? true : false;

    if (funcionario.dadosContratuais.statusAtividade == 0) {
        while (1) {
            printf("Digite a data de desligamento do funcionário (dd/mm/aaaa): ");
            int lidos = scanf("%d/%d/%d", &funcionario.dadosContratuais.dataDesligamento.dia,
                              &funcionario.dadosContratuais.dataDesligamento.mes,
                              &funcionario.dadosContratuais.dataDesligamento.ano);
            while (getchar() != '\n')
                ;

            if (lidos == 3) {
                bool dataValida = confereData(funcionario.dataNascimento);
                if (dataValida) {
                    // Sai do loop se o formato e a data forem válidos
                    break;
                } else {
                    erro("Data inválida. Por favor, insira novamente.");
                }
            } else {
                erro("Formato incorreto. Utilize apenas números no formato dd/mm/aaaa.");
            }
        }
    } else {
        funcionario.dadosContratuais.dataDesligamento.dia = 0;
        funcionario.dadosContratuais.dataDesligamento.mes = 0;
        funcionario.dadosContratuais.dataDesligamento.ano = 0;
    }
    printf("Digite o historico de pagamentos do funcionario (12 meses):\n");
    for (i = 0; i < 12; i++) {
        while (1) {
            printf("Mes %d: ", i + 1);
            int lidos = scanf("%f", &funcionario.historicoPagamentos[i]);
            // Limpa buffer
            while (getchar() != '\n')
                ;
            // se for o valor correto vai apra o próximo mês
            if (lidos == 1) {
                break;
            } else {
                erro("Valor inválido. Por favor, digite um número financeiro válido.");
            }
        }
    }

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

    // verifica a existencia dos parametros.
    if (!arquivo || !cabecalho) {
        // se utiliza da função de erro para formatar a mensagem apresentando erro
        erro("Arquivo e cabecalho não criados corretamente na hora de excluir funcionario");
        return;
    }

    Chave chave;
    Funcionario existente;
    bool removido  = false;
    bool encontrou = false;
    long posfolha;
    int i;

    // Recebe as informações do funcionario que sera excluidp
    printf("Digite o nome do funcionario:");
    fgets(chave.Nome, RH_TAM_NOME, stdin);
    chave.Nome[strcspn(chave.Nome, "\n")] = '\0';

    chave.Nome[strcspn(chave.Nome, "\n")] = '\0';
    chave.dataNascimento.dia              = 0;
    chave.dataNascimento.mes              = 0;
    chave.dataNascimento.ano              = 0;
    printf("\n");
    Funcionario encontrado;

    int qtdChaves = buscarPorIntervalo(arquivo, cabecalho, &chave, &chave, comparaNome, imprimeChave, false, &encontrado);
    
    printf("\n");
    if (qtdChaves <= 0) {
        printf("Nenhum funcionário encontrado com o nome '%s'.\n", chave.Nome);
    } else {
        if (qtdChaves == 1) {
            printf("Tem certeza que deseja remover: '%s'? (0.Não  1.Sim)\n>> ", chave.Nome);
            short int aux = 1;
            scanf("%hd", &aux);
            while (getchar() != '\n');
            if (aux >= 1) {
                chave.dataNascimento.dia = encontrado.dataNascimento.dia;
                chave.dataNascimento.mes = encontrado.dataNascimento.mes;
                chave.dataNascimento.ano = encontrado.dataNascimento.ano;
                removido = removeChave(arquivo, cabecalho, &chave, comparaChaveFuncionario);
            } else {
                printf("Voltando ao menu inicial....\n");
                return;
            }
        } else {
            printf("Foram encontrados %d funcionários com o nome '%s'.\n", qtdChaves, chave.Nome);
            // recebe os dados cadastrais do funcionario
            while (1) {
                printf("Digite a data de nascimento do funcionário (dd/mm/aaaa): ");
                int lidos = scanf("%d/%d/%d", &chave.dataNascimento.dia, &chave.dataNascimento.mes,
                                  &chave.dataNascimento.ano);
                // limpar buffer
                //while (getchar() != '\n');

                if (lidos == 3) {
                    bool dataValida = confereData(chave.dataNascimento);
                    if (dataValida) {
                        // Sai do loop se o formato e a data forem válidos
                        break;
                    } else {
                        erro("Data inválida. Por favor, insira novamente.");
                    }
                } else {
                    erro("Formato incorreto. Utilize apenas números no formato dd/mm/aaaa.");
                }
            }
            imprimeChave(&chave);
            removido = removeChave(arquivo, cabecalho, &chave, comparaChaveFuncionario);
        }
    }

    // retorna uma mensagem de exito, se utilizando da função exito, para exibir
    // a mensagem de sucesso ao usuario.
    if (removido) {
        exito("Funcionario removido com sucesso!");
    } else {
        erro("Não foi possível remover");
    }
}


void buscaIntervaloFuncionario(FILE *arquivo, CabecalhoBPlus *cabecalho) {

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
    Funcionario funcionario;
    buscarPorIntervalo(arquivo, cabecalho, &chaveA, &chaveB, comparaNome, imprimeFuncionario, false, &funcionario);
}

void buscaFuncionario(FILE *arquivo, CabecalhoBPlus *cabecalho) {

    // verifica se os parametros são validos
    if (!arquivo || !cabecalho)
        erro("Parametros invalidos passados para buscaFuncionario");

    Chave chaveA;
    Funcionario funcionario;
    printf("Digite o nome do funcionario que deseja buscar: ");
    fgets(chaveA.Nome, RH_TAM_NOME, stdin);
    chaveA.Nome[strcspn(chaveA.Nome, "\n")] = '\0';
    strcpy(funcionario.Nome, chaveA.Nome);
    chaveA.dataNascimento.dia               = 0;
    chaveA.dataNascimento.mes               = 0;
    chaveA.dataNascimento.ano               = 0;
    printf("\n");
    int qtdChaves = buscarPorIntervalo(arquivo, cabecalho, &chaveA, &chaveA, comparaNome,
                                       imprimeChave, true, &funcionario);
    if (qtdChaves == 0) {
        printf("Nenhum funcionário encontrado com o nome '%s'.\n", chaveA.Nome);
    } else {
        if (qtdChaves == 1) {
            imprimeFuncionario(&funcionario);
            return;
        } else {
            printf("Foram encontrados %d funcionários com o nome '%s'.\n", qtdChaves, chaveA.Nome);
            printf("Digite a data de aniversário do funcionário que deseja buscar "
                   "(dd/mm/aaaa): ");
            scanf("%d/%d/%d", &chaveA.dataNascimento.dia, &chaveA.dataNascimento.mes,
                  &chaveA.dataNascimento.ano);
            buscarPorIntervalo(arquivo, cabecalho, &chaveA, &chaveA, comparaChaveFuncionario, imprimeFuncionario, false, &funcionario);
        }
    }
}

void imprimeArvoreFuncionario(FILE *arquivo, CabecalhoBPlus *cabecalho) {
    bool ehUltimo      = false;
    char prefixo[100]  = "";
    long posisaoPagina = cabecalho->posRaiz;
    imprimeBPlus(arquivo, cabecalho, posisaoPagina, prefixo, ehUltimo, imprimeChavePrimeiroNome);
}