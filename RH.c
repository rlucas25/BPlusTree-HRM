// Turma: 3704.1
// Grupo 14
// DENNIS FRANCISCO GUIMARÃES DE OLIVEIRA BARACHO; 2024200496
// HEITOR SANTOS MAIA; 2024200498
// LUCAS REBOUÇAS ALMEIDA; 2024200501
// MARCOS VINÍCIUS PIMENTEL GOMES; 2024200500

#include "RH.h"
#include "Bplus.h"
#include <stdio.h>

void imprimeData(Data data) { printf("%d/%d/%d", data.dia, data.mes, data.ano); }

void imprimeChave(void *f) {

    // Impressão das chaves de determinado funcionario
    Chave *chave = (Chave *)f;
    printf("[Nome: %s | Data de nascimento: ", chave->Nome);
    imprimeData(chave->dataNascimento);
    printf("]");
}
void imprimeChavePrimeiroNome(void *f) {
    // Impressão das chaves de determinado funcionario
    Chave *chave = (Chave *)f;

    int tamanho = strcspn(chave->Nome, " ");

    printf("[%.*s | ", tamanho, chave->Nome);
    imprimeData(chave->dataNascimento);
    printf("]");
}

void imprimeFuncionario(void *f) {
    Funcionario funcionario = *(Funcionario *)f;

    // Formatação da impressão dos dados.
    printf("\n| Nome: %s\n| Data de nascimento: ", funcionario.Nome);
    imprimeData(funcionario.dataNascimento);
    printf("\n| Pai: %s\n| Mãe: %s\n", funcionario.filicao.pai, funcionario.filicao.mae);
    printf("| Dados Contratuais:\n");
    printf("|\t Data de Contratação: ");
    imprimeData(funcionario.dadosContratuais.dataContratacao);
    printf("\n");
    printf("|\t Status de atividade: ");

    // Verifica se o funcionario está ativo, se sim, imprime o status: Ativo
    if (funcionario.dadosContratuais.statusAtividade) {
        printf("Ativo\n");
    } else {
        // Se não, imprime o status: Inativo, e demonstra data de desligamento.
        printf("Inativo|\tData de Desligamento: ");
        imprimeData(funcionario.dadosContratuais.dataDesligamento);
        printf("\n");
    }
    // Formatação das outras impressões de dados.
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

    // Verifica se as informações inseridas estão corretas em relação a data atual.
    if (data.dia < 1 || data.dia > 31 || data.mes < 1 || data.mes > 12 || data.ano > 2026) {
        return false;
    }
    // Verifica se as informações inseridas estão corretas em relação ao dia do mês.
    if ((data.mes == 4 || data.mes == 6 || data.mes == 9 || data.mes == 11) && data.dia > 30) {
        return false;
    }

    // Trata o caso do mês de fevereiro e ano bissexto.
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

    // Não permite meses além do possível, segue o ano atual.
    if (data.ano == 2026 && data.mes > 7) {
        return false;
    }

    return true;
}

int comparaNome(void *a, void *b) {
    // Inicializa as variáveis que serão utilizadas na comparação posterior.
    const Chave *chaveA = (const Chave *)a;
    const Chave *chaveB = (const Chave *)b;

    // Retorna a diferença entre o nome da chaveA e a chaveB.
    return strcmp(chaveA->Nome, chaveB->Nome);
}
int comparaChaveFuncionario(void *a, void *b) {

    int resultadoNome = comparaNome(a, b);

    // Verifica se os nomes comparados são iguais.
    if (resultadoNome != 0) {

        return resultadoNome;
    }

    Chave *chaveA = (Chave *)a;
    Chave *chaveB = (Chave *)b;

    // Verifica se o anos de nascimento dos dois funcionários são diferentes, se
    // sim, retorna a diferença entre os dois.
    if (chaveA->dataNascimento.ano != chaveB->dataNascimento.ano) {

        return chaveA->dataNascimento.ano - chaveB->dataNascimento.ano;
    }

    // Verifica se os meses de nascimento dos dois funcionarios são diferentes, se
    // sim, retorna a diferença entre os dois.
    if (chaveA->dataNascimento.mes != chaveB->dataNascimento.mes) {

        return chaveA->dataNascimento.mes - chaveB->dataNascimento.mes;
    }

    // Retorna a diferença entre os dias de nascença dos funcionários.
    return chaveA->dataNascimento.dia - chaveB->dataNascimento.dia;
}

void cadastrarFuncionario(FILE *arquivo, CabecalhoBPlus *cabecalho) {
    int atualiza = 0;
    // Verifica a existencia dos parâmetros.
    if (!arquivo || !cabecalho) {
        // Utiliza a função de erro para formatar a mensagem apresentando erro de 
        // ausência de arquivo e cabeçalho.
        erro("Arquivo e cabecalho não criados corretamente na hora de cadastrar "
             "funcionario");
        return;
    }

    Funcionario funcionario;
    Funcionario existente;
    bool encontrou = false;
    long posfolha;
    int i;

    // Recebe do usuário as informações do funcionário que será cadastrado.
    printf("Digite o nome do funcionario:");
    fgets(funcionario.Nome, RH_TAM_NOME, stdin);
    funcionario.Nome[strcspn(funcionario.Nome, "\n")] = '\0';

    // Recebe os dados cadastrais do funcionario.
    while (1) {
        printf("Digite a data de nascimento do funcionário (dd/mm/aaaa): ");
        int lidos = scanf("%d/%d/%d", &funcionario.dataNascimento.dia,
                          &funcionario.dataNascimento.mes, &funcionario.dataNascimento.ano);
        // Utiliza a função "getchar()" para a limpeza do buffer.
        while (getchar() != '\n');
            

        if (lidos == 3) {
            bool dataValida = confereData(funcionario.dataNascimento);
            if (dataValida) {
                // Sai do loop de verificação se o formato e a data forem válidos.
                break;
            } else {
                erro("Data inválida. Por favor, insira novamente.");
            }
        } else {
            erro("Formato incorreto. Utilize apenas números no formato dd/mm/aaaa.");
        }
    }
    // Atribui os dados recebidos a chave de funcionário.
    Chave chaveFuncionario;
    strcpy(chaveFuncionario.Nome, funcionario.Nome);
    chaveFuncionario.dataNascimento.dia = funcionario.dataNascimento.dia;
    chaveFuncionario.dataNascimento.mes = funcionario.dataNascimento.mes;
    chaveFuncionario.dataNascimento.ano = funcionario.dataNascimento.ano;

    // Realiza a busca por funcionários de mesmo nome e data de nascimento
    // de acordo com o que foi inserido pelo usuário.
    int qtdChaves =
        buscarPorIntervalo(arquivo, cabecalho, &chaveFuncionario, &chaveFuncionario,
                           comparaChaveFuncionario, imprimeFuncionario, false, &existente);

    // Caso haja apenas 1 chave igual, pergunta se o usuário deseja atualizar.
    // o funcionário já presente no sistema.
    if (qtdChaves == 1) {
        printf("Deseja atualizar o funcionario?\n0. Não\n1. Sim\n>> ");

        scanf("%d", &atualiza);
        // Utiliza a função "getchar()" para limpeza do buffer.
        while (getchar() != '\n');
            

        // Se o usuário opta por atualizar, remove a chave antiga do sistema.
        if (atualiza >= 1) {
            removeChave(arquivo, cabecalho, &chaveFuncionario, comparaChaveFuncionario);
        } else {
            return;
        }
    }

    // Recebe as informações do funcionário a ser cadastrado ou 
    // atualizado, inseridas pelo usuário.
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
                // Sai do loop de verificação de data se o formato e a data forem válidos.
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
        printf("Digite o status de atividade do funcionario:\n1. Ativo\n0. Inativo\n>>");
        int lidos = scanf("%d", &aux);

        // Utiliza a função "getchar()" para limpeza de buffer
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

    // Caso o funcionário esteja inativo, exige a data de desligamento
    if (funcionario.dadosContratuais.statusAtividade == 0) {
        while (1) {
            printf("Digite a data de desligamento do funcionário (dd/mm/aaaa): ");
            int lidos = scanf("%d/%d/%d", &funcionario.dadosContratuais.dataDesligamento.dia,
                              &funcionario.dadosContratuais.dataDesligamento.mes,
                              &funcionario.dadosContratuais.dataDesligamento.ano);
            while (getchar() != '\n');

            if (lidos == 3) {
                bool dataValida = confereData(funcionario.dataNascimento);
                if (dataValida) {
                    // Sai do loop de verificação de data se o formato e a data forem válidos
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
    if (atualiza >= 1) {
        printf("Digite o historico de pagamentos do funcionario (12 meses):\n");
        for (i = 0; i < 12; i++) {
            while (1) {
                printf("Mes %d: ", i + 1);
                int lidos = scanf("%f", &funcionario.historicoPagamentos[i]);
                // Limpa buffer
                while (getchar() != '\n');
                // se for o valor correto vai apra o próximo mês
                if (lidos == 1) {
                    break;
                } else {
                    erro("Valor inválido. Por favor, digite um número financeiro válido.");
                }
            }
        }
    }
    else {
        for (i = 0; i < 12; i++) {
            funcionario.historicoPagamentos[i] = 0;
        }
    }

    // Se utiliza da função insereChave para inserir e verificar a possibilidade
    // de inserção, retornado TRUE, se foi bem sucedida, e FALSE, se mal sucedida.
    encontrou =
        insereChave(arquivo, cabecalho, &chaveFuncionario, &funcionario, comparaChaveFuncionario);

    // Retorna uma mensagem de êxito caso a inserção tenha sido bem sucedida,
    // do contrário, reporta erro no cadastro.
    if (encontrou) {
        exito("Funcionario cadastrado com sucesso!");
    } else {
        erro("Não foi possível cadastrar.");
    }
}

void excluirFuncionario(FILE *arquivo, CabecalhoBPlus *cabecalho) {

    // Verifica a existência dos parâmetros.
    if (!arquivo || !cabecalho) {
        // Se utiliza da função de erro para formatar a mensagem apresentando erro.
        erro("Arquivo e cabecalho não criados corretamente na hora de excluir funcionario");
        return;
    }

    Chave chave;
    Funcionario existente;
    bool removido  = false;
    bool encontrou = false;
    long posfolha;
    int i;

    // Recebe as informações do funcionário que será excluído.
    printf("Digite o nome do funcionario:");
    fgets(chave.Nome, RH_TAM_NOME, stdin);
    chave.Nome[strcspn(chave.Nome, "\n")] = '\0';

    chave.Nome[strcspn(chave.Nome, "\n")] = '\0';
    chave.dataNascimento.dia              = 0;
    chave.dataNascimento.mes              = 0;
    chave.dataNascimento.ano              = 0;
    printf("\n");
    Funcionario encontrado;

    // Realiza busca por funcionários com o nome e ano de nascimento
    // inserido pelo usuário.
    int qtdChaves = buscarPorIntervalo(arquivo, cabecalho, &chave, &chave, comparaNome,
                                       imprimeChave, false, &encontrado);

    printf("\n");
    // Caso não encontre nenhum funcionário com o respectivo nome 
    // e data de nascimento, informa ao usuário.
    if (qtdChaves <= 0) {
        printf("Nenhum funcionário encontrado com o nome '%s'.\n", chave.Nome);
    } else {
        // Caso encontre um único funcionário com o nome e a data de nascimento
        // correspondente, pergunta se o usuário deseja realmente removê-lo.
        if (qtdChaves == 1) {
            printf("Tem certeza que deseja remover: '%s'?\n0. Não\n1. Sim\n>> ", chave.Nome);
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
            // Caso haja mais de 1 funcionário com essas informações, verifica
            // qual o usuário deseja excluir.
            printf("Foram encontrados %d funcionários com o nome '%s'.\n", qtdChaves, chave.Nome);
            // recebe os dados cadastrais do funcionario
            while (1) {
                printf("Digite a data de nascimento do funcionário (dd/mm/aaaa): ");
                int lidos = scanf("%d/%d/%d", &chave.dataNascimento.dia, &chave.dataNascimento.mes,
                                  &chave.dataNascimento.ano);
                // limpar buffer
                // while (getchar() != '\n');

                if (lidos == 3) {
                    bool dataValida = confereData(chave.dataNascimento);
                    if (dataValida) {
                        // Sai do loop de verificação de data se o formato e a data forem válidos.
                        break;
                    } else {
                        erro("Data inválida. Por favor, insira novamente.");
                    }
                } else {
                    erro("Formato incorreto. Utilize apenas números no formato dd/mm/aaaa.");
                }
            }
            removido = removeChave(arquivo, cabecalho, &chave, comparaChaveFuncionario);
        }
    }

    // Retorna uma mensagem de exito caso a remoção tenha sido bem sucedida.
    // Do contrário, informa exceção de erro na remoção.
    if (removido) {
        exito("Funcionario removido com sucesso!");
    } else {
        erro("Não foi possível remover");
    }
}

void buscaIntervaloFuncionario(FILE *arquivo, CabecalhoBPlus *cabecalho) {

    // Verifica se os parametros são válidos.
    if (!arquivo || !cabecalho)
        erro("Parametros invalidos passados para buscaIntervalo");

    Chave chaveA, chaveB;

    // Obtém oss nomes dos funcionários que serão limite 
    // inferior e superior, repectivamente.
    printf("Digite o nome do funcionario que deseja buscar (inicio do intervalo): ");
    fgets(chaveA.Nome, RH_TAM_NOME, stdin);
    chaveA.Nome[strcspn(chaveA.Nome, "\n")] = '\0';
    printf("Digite o nome do funcionario que deseja buscar (fim do intervalo): ");
    fgets(chaveB.Nome, RH_TAM_NOME, stdin);
    chaveB.Nome[strcspn(chaveB.Nome, "\n")] = '\0';
    Funcionario funcionario;
    // Realiza a busca por intervalo.
    buscarPorIntervalo(arquivo, cabecalho, &chaveA, &chaveB, comparaNome, imprimeFuncionario, false,
                       &funcionario);
}

void buscaFuncionario(FILE *arquivo, CabecalhoBPlus *cabecalho) {

    // Verifica se os parametros são válidos.
    if (!arquivo || !cabecalho)
        erro("Parametros invalidos passados para buscaFuncionario");

    Chave chaveA;
    Funcionario funcionario;
    printf("Digite o nome do funcionario que deseja buscar: ");
    fgets(chaveA.Nome, RH_TAM_NOME, stdin);
    chaveA.Nome[strcspn(chaveA.Nome, "\n")] = '\0';
    strcpy(funcionario.Nome, chaveA.Nome);
    chaveA.dataNascimento.dia = 0;
    chaveA.dataNascimento.mes = 0;
    chaveA.dataNascimento.ano = 0;
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
            buscarPorIntervalo(arquivo, cabecalho, &chaveA, &chaveA, comparaChaveFuncionario,
                               imprimeFuncionario, false, &funcionario);
        }
    }
}

void imprimeArvoreFuncionario(FILE *arquivo, CabecalhoBPlus *cabecalho) {
    bool ehUltimo      = false;
    char prefixo[100]  = " ";
    long posisaoPagina = cabecalho->posRaiz;
    imprimeBPlus(arquivo, cabecalho, posisaoPagina, prefixo, ehUltimo, imprimeChavePrimeiroNome);
}