#include "Bplus.h"
#include "RH.h"

int main() {

    int opcao;
    // Cria a arvore B+ para armazenar os funcionarios, caso ela não exista e abre
    // o arquivo para manipulação
    
    criaArvore(NOME_ARQUIVO, ORDEM_P, sizeof(Chave), sizeof(Funcionario));
    CabecalhoBPlus *cabecalho = mallocSafe(sizeof(CabecalhoBPlus));
    FILE *arquivo             = abrirArvore(NOME_ARQUIVO, cabecalho);

    do {
        printf("\n\t=== MENU ===\n"
               "\t1. Inserir Funcionário\n"
               "\t2. Buscar Funcionário\n"
               "\t3. Excluir Funcionário\n"
               "\t4. Listagem por Intervalo\n"
               "\t5. Exibir Estrutura do Índice\n"
               "\t6. Sair\n");

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
            buscaIntervalo(arquivo, cabecalho);

            break;
        case 5:
            bool ehUltimo      = false;
            char prefixo[100]  = "";
            long posisaoPagina = cabecalho->posRaiz;
            imprimeBPlus(arquivo, cabecalho, posisaoPagina, prefixo, ehUltimo, imprimeChave);

            break;
        case 6:
            free(cabecalho);
            fecharArvore(arquivo);
            printf("Encerrando Programa...\n");
            return 0;

            break;
        default:
            printf("Opção Invalida! \n");
            break;
        }
    } while (opcao != 6);
}