#include "Bplus.h"
#include "RH.h"

int main() {

    int opcao;
    CabecalhoBPlus *cabecalho = mallocSafe(sizeof(CabecalhoBPlus));
    FILE *arquivo             = NULL;
    // Cria a arvore B+ para armazenar os funcionarios, caso ela não exista e abre
    // o arquivo para manipulação
    FILE *testeExistencia = fopen(NOME_ARQUIVO, "rb");
    if (!testeExistencia) {
        criaArvore(NOME_ARQUIVO, ORDEM_P, sizeof(Chave), sizeof(Funcionario));
    } else {
        exito("Arquivo de funcionarios existente, abrindo...");
        fclose(testeExistencia);
    }
    arquivo = abrirArvore(NOME_ARQUIVO, cabecalho);

    do {
        printf("\n\t===== MENU =====\n"
               "1. Inserir Funcionário\n"
               "2. Buscar Funcionário\n"
               "3. Excluir Funcionário\n"
               "4. Listagem por Intervalo\n"
               "5. Exibir Estrutura do Índice\n"
               "6. Sair\n");

        printf(">> ");
        scanf("%d", &opcao);
        // utilizado para limpar o buffer
        while (getchar() != '\n');

        switch (opcao) {
        case 1:
            cadastrarFuncionario(arquivo, cabecalho);
            break;
        case 2:
            buscaFuncionario(arquivo, cabecalho);
            break;
        case 3:
            excluirFuncionario(arquivo, cabecalho);
            break;
        case 4:
            char inicioIntervalo[RH_TAM_NOME], fimIntervalo[RH_TAM_NOME];
            buscaIntervaloFuncionario(arquivo, cabecalho);

            break;
        case 5:
            imprimeArvoreFuncionario(arquivo, cabecalho);
            // imprimeBPlus(arquivo, cabecalho, posisaoPagina, prefixo, ehUltimo, imprimeChave);

            break;
        case 6:
            free(cabecalho);
            fecharArvore(arquivo);
            printf("Encerrando Programa...\n");
            return 0;

            break;
        default:
            erro("Opção Invalida!");
            break;
        }
    } while (opcao != 6);
}