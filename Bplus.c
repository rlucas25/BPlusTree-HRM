#include "Bplus.h"
#include <stdio.h>
#include <string.h>

// Auxiliares para contabilização de chaves das páginas, utilizando a ordem página.
int maxChavesPagina(CabecalhoBPlus *cabecalho, bool ehfolha) {
    if (ehfolha) {
        return cabecalho->ordemP;
    } else {
        return cabecalho->ordemP - 1;
    }
}

int metadeArredondada(CabecalhoBPlus *cabecalho) { return (cabecalho->ordemP + 1) / 2; }

int minChavesPagina(CabecalhoBPlus *cabecalho, bool ehfolha) {
    int metade = metadeArredondada(cabecalho);
    if (ehfolha) {
        return metade;
    } else {
        return metade - 1;
    }
}

bool criaArvore(const char *nomeArquivo, int ordem, size_t tamanhoChave, size_t tamanhoRegistro) {

    if (!nomeArquivo || !tamanhoChave || !tamanhoRegistro) {
        erro("Parametros invalidos passados para criaArvore");
        return false;
    }

    FILE *arquivo = fopen(nomeArquivo, "wb");

    if (!arquivo) {
        erro("Não foi possivel abrir arquivo em criaArvore");
        return false;
    }

    CabecalhoBPlus cabecalho;
    cabecalho.posRaiz         = Nulo;
    cabecalho.ordemP          = ordem;
    cabecalho.tamanhoChave    = tamanhoChave;
    cabecalho.tamanhoRegistro = tamanhoRegistro;
    cabecalho.proxNolivre     = Nulo;

    fseek(arquivo, 0, SEEK_SET);
    size_t gravados = fwrite(&cabecalho, sizeof(CabecalhoBPlus), 1, arquivo);
    fclose(arquivo);
    return (gravados > 0);
}

FILE *abrirArvore(const char *nomeArquivo, CabecalhoBPlus *cabecalho) {

    if (!nomeArquivo || !cabecalho) {
        erro("Parametros invalidos passados para abrirArvore");
        return NULL;
    }

    FILE *arquivo = fopen(nomeArquivo, "rb+");

    if (!arquivo) {
        erro("Não foi possivel abrir arquivo em abrirArvore");
        return NULL;
    }
    fseek(arquivo, 0, SEEK_SET);
    fread(cabecalho, sizeof(CabecalhoBPlus), 1, arquivo);

    return arquivo;
}

void fecharArvore(FILE *arquivoAberto) {
    if (!arquivoAberto) {
        erro("Arquivo nulo ou ja fechado em fecharArvore");
        return;
    }

    fclose(arquivoAberto);
}

// lendo a página para não integrar a os ponteiros void na leitura do read dele.
PaginaBPlus le_pagina_disco(FILE *arquivo, long offset, CabecalhoBPlus *cabecalho) {

    PaginaBPlus pagina;
    // Faz o cursor do arquivo apontar para onde a página vai ser salva
    fseek(arquivo, offset, SEEK_SET);
    // Faz a leitura dos dados da página
    fread(&pagina.qtdChaves, sizeof(int), 1, arquivo);
    fread(&pagina.ehfolha, sizeof(bool), 1, arquivo);
    fread(&pagina.proximaFolha, sizeof(long), 1, arquivo);
    // Aloca um espaço na memória RAM para salvar as chaves no disco
    pagina.chaves = calloc(maxChavesPagina(cabecalho, pagina.ehfolha) + 1, cabecalho->tamanhoChave);
    if (!pagina.chaves) {
        erro("Falha de alocacao para chaves em le_pagina_disco\n");
        return pagina;
    }
    // Aloca um espaço na memória RAM para receber os ponteiros no disco, faz um if para verificar
    // se é folha ou se é nó interno.
    if (pagina.ehfolha) {
        pagina.ponteiros =
            calloc(maxChavesPagina(cabecalho, pagina.ehfolha) + 2, cabecalho->tamanhoRegistro);
    } else {
        pagina.ponteiros = calloc(maxChavesPagina(cabecalho, pagina.ehfolha) + 2, sizeof(long));
    }

    if (!pagina.ponteiros) {
        erro("Falha de alocacao para ponteiros em le_pagina_disco\n");
        free(pagina.chaves);
        return pagina;
    }

    // Agora que realmente inicializou as coisas e está salvo na memória RAM, carrega os dados do
    // disco para os vetores
    fread(pagina.chaves, cabecalho->tamanhoChave, maxChavesPagina(cabecalho, pagina.ehfolha),
          arquivo);

    if (pagina.ehfolha) {
        // se for folha carrega os registros
        fread(pagina.ponteiros, cabecalho->tamanhoRegistro,
              maxChavesPagina(cabecalho, pagina.ehfolha) + 1, arquivo);
    } else {
        // Se for nó interno carrega os ponteiros/offsets dos filhos
        fread(pagina.ponteiros, sizeof(long), maxChavesPagina(cabecalho, pagina.ehfolha) + 1,
              arquivo);
    }

    return pagina;
}

bool escreve_pagina_disco(FILE *arquivo, long offset, PaginaBPlus *pagina,
                          CabecalhoBPlus *cabecalho) {
    printf("Escrevendo pagina na posicao: %ld (Eh folha? %s | Qtd Chaves: %ld)\n", offset,
           pagina->ehfolha ? "SIM" : "NAO", pagina->qtdChaves);
    fseek(arquivo, offset, SEEK_SET);
    fwrite(&pagina->qtdChaves, sizeof(int), 1, arquivo);
    fwrite(&pagina->ehfolha, sizeof(bool), 1, arquivo);
    fwrite(&pagina->proximaFolha, sizeof(long), 1, arquivo);

    fwrite(pagina->chaves, cabecalho->tamanhoChave, maxChavesPagina(cabecalho, pagina->ehfolha),
           arquivo);

    if (pagina->ehfolha) {
        fwrite(pagina->ponteiros, cabecalho->tamanhoRegistro,
               maxChavesPagina(cabecalho, pagina->ehfolha) + 1, arquivo);
    } else {
        fwrite(pagina->ponteiros, sizeof(long), maxChavesPagina(cabecalho, pagina->ehfolha) + 1,
               arquivo);
    }

    return true;
}

long buscarChave(FILE *arquivoAberto, bool *encontrou, CabecalhoBPlus *cabecalho, void *chave,
                 int (*comparaChaves)(void *x, void *y), long *caminho, int *tam_caminho) {

    if (!arquivoAberto || !cabecalho || !chave || !comparaChaves || !encontrou) {
        erro("Parametro invalidos passados para srChave");
        return -1;
    }

    long posicaoAtual = cabecalho->posRaiz;
    int i;
    *encontrou = false;

    *tam_caminho = 0; // Começamos com o caminho vazio

    if (posicaoAtual == Nulo) {
        return Nulo;
    }

    while (posicaoAtual != Nulo) {

        PaginaBPlus paginaAtual = le_pagina_disco(arquivoAberto, posicaoAtual, cabecalho);
        i                       = 0;

        while (i < paginaAtual.qtdChaves &&
               comparaChaves(chave, (char *)paginaAtual.chaves + (i * cabecalho->tamanhoChave)) >
                   0) {
            i++;
        }

        if (paginaAtual.ehfolha) {

            if (i < paginaAtual.qtdChaves &&
                comparaChaves(chave, (char *)paginaAtual.chaves + (i * cabecalho->tamanhoChave)) ==
                    0) {

                *encontrou = true;

            } else {

                *encontrou = false;
            }

            free(paginaAtual.chaves);
            free(paginaAtual.ponteiros);
            return posicaoAtual;
        }

        if (caminho != NULL && tam_caminho != NULL) {
            caminho[*tam_caminho] = posicaoAtual;
            (*tam_caminho)++;
        }
        posicaoAtual = *((long *)paginaAtual.ponteiros + i);
        free(paginaAtual.chaves);
        free(paginaAtual.ponteiros);
    }
    printf("Erro na Busca");
    return -1;
}

PaginaBPlus *cisaoPagina(PaginaBPlus *pagina, PaginaBPlus *paginaPai, CabecalhoBPlus *cabecalho,
                         void *chavePromovida) {

    PaginaBPlus *novaPagina;
    int totalChaves;
    int meioIndex;
    int i;

    if (!pagina || !cabecalho || !chavePromovida) {
        erro("Parametros invalidos passados para cisaoPagina");
        return NULL;
    }

    novaPagina               = mallocSafe(sizeof(PaginaBPlus));
    novaPagina->ehfolha      = pagina->ehfolha;
    novaPagina->qtdChaves    = 0;
    novaPagina->proximaFolha = Nulo;

    novaPagina->chaves =
        mallocSafe((maxChavesPagina(cabecalho, novaPagina->ehfolha) + 1) * cabecalho->tamanhoChave);
    if (novaPagina->ehfolha) {
        novaPagina->ponteiros = mallocSafe((maxChavesPagina(cabecalho, novaPagina->ehfolha) + 1) *
                                           cabecalho->tamanhoRegistro);
    } else {
        novaPagina->ponteiros =
            mallocSafe((maxChavesPagina(cabecalho, novaPagina->ehfolha) + 1) * sizeof(long));
    }

    // Pagina em overflow: qtdChaves == ordem + 1 (indices 0..ordem)
    totalChaves = pagina->qtdChaves;
    meioIndex   = totalChaves / 2;

    if (pagina->ehfolha) {
        // Quando é folha, a cisão é diferente do que quando é página interna,
        // Em folhas a chave do meio não é removida, então ela é duplicada, ficando como a primeira
        // chave da nova pagina da direita e tambem sobe pro pai.
        memcpy(chavePromovida, (char *)pagina->chaves + (meioIndex * cabecalho->tamanhoChave),
               cabecalho->tamanhoChave);

        for (i = meioIndex; i < totalChaves; i++) {
            memcpy((char *)novaPagina->chaves + (novaPagina->qtdChaves * cabecalho->tamanhoChave),
                   (char *)pagina->chaves + (i * cabecalho->tamanhoChave), cabecalho->tamanhoChave);

            memcpy((char *)novaPagina->ponteiros +
                       (novaPagina->qtdChaves * cabecalho->tamanhoRegistro),
                   (char *)pagina->ponteiros + (i * cabecalho->tamanhoRegistro),
                   cabecalho->tamanhoRegistro);

            novaPagina->qtdChaves++;
        }

        pagina->qtdChaves = meioIndex;

    } else {
        // Quando é página interna, a cisão é diferente da folha,
        // Aqui a chave do meio é removida do nó e sobe sozinha para o pai.
        memcpy(chavePromovida, (char *)pagina->chaves + (meioIndex * cabecalho->tamanhoChave),
               cabecalho->tamanhoChave);

        // Aqui faz a copia das chaves a direita da chave promovida
        for (i = meioIndex + 1; i < totalChaves; i++) {
            memcpy((char *)novaPagina->chaves + (novaPagina->qtdChaves * cabecalho->tamanhoChave),
                   (char *)pagina->chaves + (i * cabecalho->tamanhoChave), cabecalho->tamanhoChave);
            novaPagina->qtdChaves++;
        }

        // Aqui também faz copia, mas para  filhos correspondentes.
        for (i = meioIndex + 1; i <= totalChaves; i++) {
            memcpy((char *)novaPagina->ponteiros + ((i - meioIndex - 1) * sizeof(long)),
                   (char *)pagina->ponteiros + (i * sizeof(long)), sizeof(long));
        }

        pagina->qtdChaves = meioIndex;
    }

    // NAO insere chavePromovida em paginaPai aqui: quem faz isso, de forma
    // ordenada (deslocando chaves/ponteiros), e o insereChave logo depois
    // de chamar essa funcao. Inserir aqui tambem causava chave duplicada
    // e qtdChaves incrementado duas vezes no pai.
    (void)paginaPai;

    return novaPagina;
}

// Aloca uma posicao no disco para uma nova pagina, reaproveitando o
// espaco livre deixado por remocoes (lista encadeada via proxNolivre)
// antes de crescer o arquivo. Isso evita que o arquivo so aumente de
// tamanho e nunca reutilize os "buracos" deixados por BalancearArvoreUnderflow.
long alocarPagina(FILE *arquivoAberto, CabecalhoBPlus *cabecalho) {
    long posNova;

    if (cabecalho->proxNolivre != Nulo) {
        // Reaproveita o primeiro no livre da lista encadeada.
        posNova = cabecalho->proxNolivre;

        // A pagina livre guarda, no campo proximaFolha, o offset do
        // proximo no livre da lista (ou Nulo se for o ultimo).
        PaginaBPlus paginaLivre = le_pagina_disco(arquivoAberto, posNova, cabecalho);
        cabecalho->proxNolivre  = paginaLivre.proximaFolha;

        free(paginaLivre.chaves);
        free(paginaLivre.ponteiros);
    } else {
        // Lista de livres vazia: cresce o arquivo normalmente.
        fseek(arquivoAberto, 0, SEEK_END);
        posNova = ftell(arquivoAberto);
    }

    // Persiste imediatamente o cabecalho, pois proxNolivre pode ter
    // mudado aqui e o insereChave nem sempre reescreve o cabecalho no
    // disco antes de retornar (só reescrevia no caso de nova raiz).
    fseek(arquivoAberto, 0, SEEK_SET);
    fwrite(cabecalho, sizeof(CabecalhoBPlus), 1, arquivoAberto);

    return posNova;
}

bool insereChave(FILE *arquivoAberto, CabecalhoBPlus *cabecalho, void *chave, const void *registro,
                 int (*comparaChaves)(void *x, void *y)) {
    int (*compararChaves)(const void *, const void *) =
        (int (*)(const void *, const void *))comparaChaves;
    if (!arquivoAberto || !cabecalho || !chave || !registro || !comparaChaves) {
        erro("Parametros invalidos passados para insereChave");
        return false;
    }

    PaginaBPlus paginaNova;
    // Caso 1: Árvore completamente vazia
    if (cabecalho->posRaiz == Nulo) {
        // Atribuindo valores iniciais de uma página que se inicia como folha e raiz
        paginaNova.ehfolha      = true;
        paginaNova.qtdChaves    = 1;
        paginaNova.proximaFolha = Nulo;

        // Aloca espaço na memória RAM para as chaves e registros
        // Apresenta espaço para o overflow (ordem +1 e +2)
        paginaNova.chaves    = mallocSafe((maxChavesPagina(cabecalho, paginaNova.ehfolha) + 1) *
                                          cabecalho->tamanhoChave);
        paginaNova.ponteiros = mallocSafe((maxChavesPagina(cabecalho, paginaNova.ehfolha) + 2) *
                                          cabecalho->tamanhoRegistro);

        // Escrevendo no disco a nova página e o primeiro registro
        memcpy(paginaNova.chaves, chave, cabecalho->tamanhoChave);
        memcpy(paginaNova.ponteiros, registro, cabecalho->tamanhoRegistro);

        // Define a posição da raiz após o cabeçalho no disco
        cabecalho->posRaiz = sizeof(CabecalhoBPlus);
        escreve_pagina_disco(arquivoAberto, cabecalho->posRaiz, &paginaNova, cabecalho);

        // Atualiza o cabeçalho no início do disco
        fseek(arquivoAberto, 0, SEEK_SET);
        fwrite(cabecalho, sizeof(CabecalhoBPlus), 1, arquivoAberto);

        // Limpeza dos auxiliares na memória RAM
        free(paginaNova.chaves);
        free(paginaNova.ponteiros);

        return true;
    }

    // Caso 2: A árvore já possui elementos
    bool encontrou = false;
    long caminho[50];
    int tam_caminho = 0;

    // Busca a posição da folha para inserção da chave
    long posFolha = buscarChave(arquivoAberto, &encontrou, cabecalho, chave, comparaChaves, caminho,
                                &tam_caminho);

    // Caso já haja uma chave com os dados da chave a ser inserida,
    // ocasiona erro. Não são permitidas chaves iguais.
    if (encontrou) {
        erro("Chave já cadastrada");
        return false;
    }

    // Utiliza a página folha do disco na memória RAM
    PaginaBPlus paginaAtual = le_pagina_disco(arquivoAberto, posFolha, cabecalho);
    // Realiza a inserção de traz para a frente na memória
    int i = paginaAtual.qtdChaves - 1;
    while (i >= 0) {
        // Pega a chave atual e compara com a nova chave, garante que a
        // a inserção acontecerá de forma que as chaves estejam em
        // ordem crescente
        void *chaveAtual = (char *)paginaAtual.chaves + (i * cabecalho->tamanhoChave);

        // Se a nova chave for menor que a chave atual do ciclo,
        // empurra a atual para a direita
        if (compararChaves(chave, chaveAtual) < 0) {
            memcpy((char *)paginaAtual.chaves + ((i + 1) * cabecalho->tamanhoChave),
                   (char *)chaveAtual, cabecalho->tamanhoChave);
            void *regAtual = (char *)paginaAtual.ponteiros + (i * cabecalho->tamanhoRegistro);
            memcpy((char *)paginaAtual.ponteiros + ((i + 1) * cabecalho->tamanhoRegistro), regAtual,
                   cabecalho->tamanhoRegistro);
            i--;
        } else {
            break; // Encontrou a posição correta de inserção, não é necessário realizar mais
                   // avaliações.
        }
    }
    // Insere, de forma ordenada, a nova chave e registro na posição vaga
    memcpy((char *)paginaAtual.chaves + ((i + 1) * cabecalho->tamanhoChave), (char *)chave,
           cabecalho->tamanhoChave);
    memcpy((char *)paginaAtual.ponteiros + ((i + 1) * cabecalho->tamanhoRegistro), (char *)registro,
           cabecalho->tamanhoRegistro);

    paginaAtual.qtdChaves++;

    // Inicializa as variáveis para o loop da cisão
    long posAtual    = posFolha;
    int indexCaminho = tam_caminho - 1;

    // Aloca espaço para a chave que subirá para o nó pai em caso de
    // overflow.
    void *chavePromovida = mallocSafe(cabecalho->tamanhoChave);
    memcpy(chavePromovida, chave, cabecalho->tamanhoChave);

    long posNovaPaginaDireita = Nulo;

    // Verifica se há overflow, ou seja, se estourou a capacidade da página. Se sim, realiza o
    // processo de cisão de baixo para cima.
    while (paginaAtual.qtdChaves > cabecalho->ordemP - 1) {
        PaginaBPlus paginaPai;
        PaginaBPlus *novaPagina = NULL;
        long posPai             = Nulo;

        // Se indexCaminho == 0, a raiz estourou, e será necessário
        // um novo nó pai, ou seja, uma nova raiz.
        if (indexCaminho < 0) {
            paginaPai.ehfolha      = false;
            paginaPai.qtdChaves    = 0;
            paginaPai.proximaFolha = Nulo;

            paginaPai.chaves = mallocSafe((maxChavesPagina(cabecalho, paginaPai.ehfolha) + 1) *
                                          cabecalho->tamanhoChave);
            paginaPai.ponteiros =
                mallocSafe((maxChavesPagina(cabecalho, paginaPai.ehfolha) + 2) * sizeof(long));

            long *ponteirosPai = (long *)paginaPai.ponteiros;
            ponteirosPai[0]    = posAtual;
        } else {
            // Se possui um pai no caminho percorrido, lê ele no disco
            posPai    = caminho[indexCaminho];
            paginaPai = le_pagina_disco(arquivoAberto, posPai, cabecalho);
        }

        // Divide-se a página atual ao meio por meio da cisão,
        // novaPagina recebe a metade direita e chave promovida
        // se recebe o elemento do meio.
        novaPagina = cisaoPagina(&paginaAtual, &paginaPai, cabecalho, chavePromovida);

        // Garante que a nova página criada a direita seja sempre
        // escrita ao fim do arquivo, preservando outros dados.
        posNovaPaginaDireita = alocarPagina(arquivoAberto, cabecalho);

        // Atualiza a lista encadeada de folhas.
        if (paginaAtual.ehfolha) {
            novaPagina->proximaFolha = paginaAtual.proximaFolha;
            paginaAtual.proximaFolha = posNovaPaginaDireita;
        }

        // Inserção da chave promovida no nó pai.
        int j              = paginaPai.qtdChaves - 1;
        long *ponteirosPai = (long *)paginaPai.ponteiros;

        // Loop responsável por arrastar, de trás para frente, as
        // chaves maiores para a direita, reorganizando os ponteiros do nó pai.
        while (j >= 0) {
            void *chavePaiAtual = (char *)paginaPai.chaves + (j * cabecalho->tamanhoChave);
            if (comparaChaves(chavePromovida, chavePaiAtual) < 0) {
                memcpy((char *)paginaPai.chaves + ((j + 1) * cabecalho->tamanhoChave),
                       chavePaiAtual, cabecalho->tamanhoChave);
                ponteirosPai[j + 2] = ponteirosPai[j + 1];
                j--;
            } else {
                break;
            }
        }

        // Insere a chave promovida no nó pai.
        memcpy((char *)paginaPai.chaves + ((j + 1) * cabecalho->tamanhoChave), chavePromovida,
               cabecalho->tamanhoChave);

        // Insere o ponteiro para a inserção de uma próxima chave a direita.
        ponteirosPai[j + 2] = posNovaPaginaDireita;
        paginaPai.qtdChaves++;

        // Salva as duas páginas resultantes da cisão: folha e página a direita
        escreve_pagina_disco(arquivoAberto, posAtual, &paginaAtual, cabecalho);
        escreve_pagina_disco(arquivoAberto, posNovaPaginaDireita, novaPagina, cabecalho);

        // limpeza de auxiliares
        free(novaPagina->chaves);
        free(novaPagina->ponteiros);
        free(novaPagina);

        free(paginaAtual.chaves);
        free(paginaAtual.ponteiros);

        // Se indexCaminho < 0, foi criada uma nova raiz para toda a
        // árvore. Portanto, os processos de cisão são finalizados.
        if (indexCaminho < 0) {
            long posNovaRaiz = alocarPagina(arquivoAberto, cabecalho);
            escreve_pagina_disco(arquivoAberto, posNovaRaiz, &paginaPai, cabecalho);

            cabecalho->posRaiz = posNovaRaiz;
            fseek(arquivoAberto, 0, SEEK_SET);
            fwrite(cabecalho, sizeof(CabecalhoBPlus), 1, arquivoAberto);

            free(paginaPai.chaves);
            free(paginaPai.ponteiros);
            free(chavePromovida);
            return true;
        }

        // Ajuste das variáveis para a próxima iteração
        paginaAtual = paginaPai;
        posAtual    = posPai;
        indexCaminho--;
    }

    // Salva a última alteração do nó atual no disco
    escreve_pagina_disco(arquivoAberto, posAtual, &paginaAtual, cabecalho);

    // Última limpeza de auxiliares
    free(paginaAtual.ponteiros);
    free(paginaAtual.chaves);
    free(chavePromovida);
    return true;
}

void redistribuicao(PaginaBPlus *atual, PaginaBPlus *irmao, bool ehEsq, int idxFilhoAtual,
                    PaginaBPlus *pai, CabecalhoBPlus *cabecalho) {

    if (atual->ehfolha) {

        if (ehEsq) {
            // Redistribuição de irmão esquerdo (folha)

            // Abre espaço no início do nó atual para 1 elemento
            memmove((char *)atual->chaves + cabecalho->tamanhoChave, atual->chaves,
                    atual->qtdChaves * cabecalho->tamanhoChave);
            memmove((char *)atual->ponteiros + cabecalho->tamanhoRegistro, atual->ponteiros,
                    atual->qtdChaves * cabecalho->tamanhoRegistro);

            // Copia a última chave e registro do irmão esquerdo para a primeira
            // posição do atual
            void *ultimaChaveIrmao =
                (char *)irmao->chaves + ((irmao->qtdChaves - 1) * cabecalho->tamanhoChave);
            void *ultimoRegIrmao =
                (char *)irmao->ponteiros + ((irmao->qtdChaves - 1) * cabecalho->tamanhoRegistro);

            memcpy(atual->chaves, ultimaChaveIrmao, cabecalho->tamanhoChave);
            memcpy(atual->ponteiros, ultimoRegIrmao, cabecalho->tamanhoRegistro);

            // Atualiza as quantidades
            irmao->qtdChaves--;
            atual->qtdChaves++;

            // Na folha, atualiza a chave do pai para refletir o novo menor elemento
            // do nó atual
            void *chavePai = (char *)pai->chaves + ((idxFilhoAtual - 1) * cabecalho->tamanhoChave);
            memcpy(chavePai, atual->chaves, cabecalho->tamanhoChave);

        } else {
            // Redistribuição de irmão direito (folha)

            // Copia a primeira chave e registro do irmão direito para o final do nó
            // atual
            void *destinoChave =
                (char *)atual->chaves + (atual->qtdChaves * cabecalho->tamanhoChave);
            void *destinoReg =
                (char *)atual->ponteiros + (atual->qtdChaves * cabecalho->tamanhoRegistro);

            memcpy(destinoChave, irmao->chaves, cabecalho->tamanhoChave);
            memcpy(destinoReg, irmao->ponteiros, cabecalho->tamanhoRegistro);

            // Desloca os elementos restantes do irmão direito para a esquerda
            memmove(irmao->chaves, (char *)irmao->chaves + cabecalho->tamanhoChave,
                    (irmao->qtdChaves - 1) * cabecalho->tamanhoChave);
            memmove(irmao->ponteiros, (char *)irmao->ponteiros + cabecalho->tamanhoRegistro,
                    (irmao->qtdChaves - 1) * cabecalho->tamanhoRegistro);

            // Atualiza as quantidades
            irmao->qtdChaves--;
            atual->qtdChaves++;

            // Na folha, atualiza a chave do pai para refletir a nova primeira chave
            // do irmão direito
            void *chavePai = (char *)pai->chaves + (idxFilhoAtual * cabecalho->tamanhoChave);
            memcpy(chavePai, irmao->chaves, cabecalho->tamanhoChave);
        }
    } else {
        // Redistribuicao para nós internos
        if (ehEsq) {
            // Abre espaço para 1 chave e 1 ponteiro (long) no início do nó atual
            memmove((char *)atual->chaves + cabecalho->tamanhoChave, atual->chaves,
                    atual->qtdChaves * cabecalho->tamanhoChave);
            memmove((long *)atual->ponteiros + 1, atual->ponteiros,
                    (atual->qtdChaves + 1) * sizeof(long));

            // A chave do pai desce para a primeira posição do atual
            void *chavePai = (char *)pai->chaves + ((idxFilhoAtual - 1) * cabecalho->tamanhoChave);
            memcpy(atual->chaves, chavePai, cabecalho->tamanhoChave);

            // O último ponteiro de filho do irmão esquerdo vira o primeiro do atual
            long *ultimoPonteiroIrmao   = (long *)irmao->ponteiros + irmao->qtdChaves;
            long *primeiroPonteiroAtual = (long *)atual->ponteiros;
            *primeiroPonteiroAtual      = *ultimoPonteiroIrmao;

            // A última chave do irmão sobe para o pai
            void *ultimaChaveIrmao =
                (char *)irmao->chaves + ((irmao->qtdChaves - 1) * cabecalho->tamanhoChave);
            memcpy(chavePai, ultimaChaveIrmao, cabecalho->tamanhoChave);

            irmao->qtdChaves--;
            atual->qtdChaves++;
        } else {
            // A chave do pai desce para o final do nó atual
            void *destinoChave =
                (char *)atual->chaves + (atual->qtdChaves * cabecalho->tamanhoChave);
            void *chavePai = (char *)pai->chaves + (idxFilhoAtual * cabecalho->tamanhoChave);
            memcpy(destinoChave, chavePai, cabecalho->tamanhoChave);

            // O primeiro ponteiro do irmão direito vira o último ponteiro do atual
            long *primeiroPonteiroIrmao = (long *)irmao->ponteiros;
            long *destinoPonteiro       = (long *)atual->ponteiros + (atual->qtdChaves + 1);
            *destinoPonteiro            = *primeiroPonteiroIrmao;

            // A primeira chave do irmão direito sobe para o pai
            memcpy(chavePai, irmao->chaves, cabecalho->tamanhoChave);

            // Desloca as chaves e ponteiros restantes do irmão direito para a
            // esquerda
            memmove(irmao->chaves, (char *)irmao->chaves + cabecalho->tamanhoChave,
                    (irmao->qtdChaves - 1) * cabecalho->tamanhoChave);
            memmove(irmao->ponteiros, (long *)irmao->ponteiros + 1,
                    irmao->qtdChaves * sizeof(long));

            irmao->qtdChaves--;
            atual->qtdChaves++;
        }
    }
}

void concatenacao(FILE *arquivo, PaginaBPlus *atual, PaginaBPlus *irmao, const void *chavePai,
                  CabecalhoBPlus *cabecalho) {

    // local onde as novas chaves começarão a ser inseridas na página atual
    void *destinoChaves = (char *)atual->chaves + (atual->qtdChaves * cabecalho->tamanhoChave);

    // Fusão para nós folhas:
    if (atual->ehfolha) {
        // copia as chaves do irmão
        memcpy(destinoChaves, irmao->chaves, irmao->qtdChaves * cabecalho->tamanhoChave);

        // Copia os registros do irmão
        void *destinoRegistros =
            (char *)atual->ponteiros + (atual->qtdChaves * cabecalho->tamanhoRegistro);
        memcpy(destinoRegistros, irmao->ponteiros, irmao->qtdChaves * cabecalho->tamanhoRegistro);

        // atualizar a lista encadeada de folhas, apontando para a folha seguinte
        atual->proximaFolha = irmao->proximaFolha;

        // atualiza a quantidade total de chaves
        atual->qtdChaves += irmao->qtdChaves;

        // fusão para nós de páginas internas
    } else {

        if (chavePai != NULL) {
            // desce a chave do pai na chave livre
            memcpy(destinoChaves, chavePai, cabecalho->tamanhoChave);
            // atualiza a chave destino para a proxima chave livre
            destinoChaves = (char *)destinoChaves + cabecalho->tamanhoChave;

            // atualiza a quantidade de chaves agora que aumentou 1 com o pai
            atual->qtdChaves++;
        }

        // copia as chaves do irmão para o nó atual
        memcpy(destinoChaves, irmao->chaves, irmao->qtdChaves * cabecalho->tamanhoChave);
        // traz o local destinado para colocar os novos ponteiros
        void *destinoPonteiros = (long *)atual->ponteiros + atual->qtdChaves;
        // Trazendo os filhos do irmão
        memcpy(destinoPonteiros, irmao->ponteiros, (irmao->qtdChaves + 1) * sizeof(long));

        // atualiza a quantidade de chaves, sendo q vai ser o atual + irmão
        atual->qtdChaves += irmao->qtdChaves;
    }
}

bool removeChave(FILE *arquivoAberto, CabecalhoBPlus *cabecalho, void *chave,
                 int (*comparaChaves)(void *x, void *y)) {
    if (!arquivoAberto || !cabecalho || !chave || !comparaChaves) {
        erro("Parametros invalidos passados para removeChave");
        return false;
    }
    // Se é árvore vazia, nada a remover
    if (cabecalho->posRaiz == Nulo) {
        return false;
    }

    bool encontrou = false;
    // Vetor para guardar os offsets dos pais.
    long caminho[50];
    int tam_caminho = 0;
    long posFolha = buscarChave(arquivoAberto, &encontrou, cabecalho, chave, comparaChaves, caminho,
                                &tam_caminho);
    // caso não tenha encontrado a chave, ela não existe na árvore
    if (!encontrou || posFolha == -1) {
        return false;
    }
    // Agora caso encontrou, faz a leitura da pagina do disco e carrega tudo da
    // folha
    PaginaBPlus folha = le_pagina_disco(arquivoAberto, posFolha, cabecalho);

    // Procura o indice exato da chave dentro da página folha
    int indice = 0;
    while (indice < folha.qtdChaves &&
           comparaChaves(chave, (char *)folha.chaves + (indice * cabecalho->tamanhoChave)) > 0) {
        indice++;
    }
    // Calcula quantos elementos existem à direita da chave que quer apagar
    int numElementosParaDeslocar = folha.qtdChaves - 1 - indice;
    // Se tiver realmente que se deslocar, então
    if (numElementosParaDeslocar > 0) {
        // Posição da chave que foi apagada
        void *destinoChave = (char *)folha.chaves + (indice * cabecalho->tamanhoChave);
        // Descobrir onde começa o bloco de chaves que precisa ser arrastado para a
        // esquerda, ou seja, uma a direita da que foi apagada
        void *origemChave = (char *)folha.chaves + ((indice + 1) * cabecalho->tamanhoChave);
        // O Deslocamento das Chaves:
        // Primeiro - pega os dados começando da chave que está à direita
        // Segundo - arrasta/cola eles por cima do buraco (sobrescrevendo a chave
        // antiga)... Terceiro - Ele copia e empurra a quantidade de
        // numElementosParaDeslocar, ou seja a quantidade das casas a direita,
        // copiando a quantidade exata de bytes que essas chaves ocupam.
        memmove(destinoChave, origemChave, numElementosParaDeslocar * cabecalho->tamanhoChave);
        // OBS: Fica sobrando uma chave a mais no final, mas não é problema, pois
        // após o if colocamos um folha.qtdChaves--; ditando q a última chave não
        // conta, que é essa replicada

        // Mesma coisa do deslocamento de chaves, porém para os registros.
        void *destinoRegistro = (char *)folha.ponteiros + (indice * cabecalho->tamanhoRegistro);
        void *origemRegistro =
            (char *)folha.ponteiros + ((indice + 1) * cabecalho->tamanhoRegistro);
        memmove(destinoRegistro, origemRegistro,
                numElementosParaDeslocar * cabecalho->tamanhoRegistro);
    }
    // Termina a exclusão de vez
    folha.qtdChaves--;

    // Verifica as regras da Árvore B+ de chaves mínimas.

    int minimoChaves = minChavesPagina(cabecalho, folha.ehfolha);

    // Cenário em que é remoção simples e não deu underflow, ou seja, não ficou
    // menos que o mínimo.
    if (folha.qtdChaves >= minimoChaves || posFolha == cabecalho->posRaiz) {
        // Sala no disco
        escreve_pagina_disco(arquivoAberto, posFolha, &folha, cabecalho);

        free(folha.chaves);
        free(folha.ponteiros);
        return true;
    }

    // Cenário em que deu underflow, ou seja, ficou menos que o mínimo:
    // Salva o estado atual da folha no disco antes de acionar o balanceamento
    escreve_pagina_disco(arquivoAberto, posFolha, &folha, cabecalho);

    // Chama a função recursiva para tratar redistribuições e fusões na folha e
    // subir até a raiz se necessário
    BalancearArvoreUnderflow(arquivoAberto, cabecalho, posFolha, caminho, tam_caminho);

    free(folha.chaves);
    free(folha.ponteiros);
    return true;
}

void BalancearArvoreUnderflow(FILE *arquivoAberto, CabecalhoBPlus *cabecalho, long posNoAtual,
                              long *caminho, int tam_caminho) {

    // RAIZ VAZIA (Diminuição da altura da árvore)
    // Ou já veio vazia ou a recursão foi subindo em cascata e chegou no topo
    // absoluto. A raiz não tem pai, então é impossível fazer redistribuição ou
    // fusão com ela, ou seja, chegamos no limite.
    if (posNoAtual == cabecalho->posRaiz) {
        PaginaBPlus raiz = le_pagina_disco(arquivoAberto, posNoAtual, cabecalho);

        // Caso raiz tinha apenas 1 chave e 2 filhos.
        // E ocorre uma fusão aqueles 2 filhos da raiz se juntaram em 1 só.
        // Então chave separadora da raiz é deletada.
        // A raiz passa a ter 0 chaves e 1 único ponteiro apontando para o filho
        // agora único
        if (!raiz.ehfolha && raiz.qtdChaves == 0) {

            // O filho restante que está em P[0] é promovido e se torna a nova Raiz da
            // árvore.
            long novaRaiz = *((long *)raiz.ponteiros + 0);

            // Atualiza no cabeçalho
            cabecalho->posRaiz = novaRaiz;

            // Atualiza a raiz antiga para ter o valor do Nó livre que foi removido
            // antes ou nulo que estava salvo no cabeçalho
            raiz.proximaFolha = cabecalho->proxNolivre;

            // Atualiza a raiz no disco
            escreve_pagina_disco(arquivoAberto, posNoAtual, &raiz, cabecalho);

            // Atualiza o cabeçalho para salvar a posição desse recém removido
            cabecalho->proxNolivre = posNoAtual;

            // Volta o ponteiro do disco para o cabeçalho
            fseek(arquivoAberto, 0, SEEK_SET);

            // Atualiza o cabeçalho no disco
            fwrite(cabecalho, sizeof(CabecalhoBPlus), 1, arquivoAberto);
        }
        // libera os ponteiros/vetores
        free(raiz.chaves);
        free(raiz.ponteiros);
        return;
    }

    // Primeiro - Carrega o nó pai do Nó Atual usando o rastro feito lá no
    // buscarChaves

    long posPai         = caminho[tam_caminho - 1];
    PaginaBPlus pai     = le_pagina_disco(arquivoAberto, posPai, cabecalho);
    PaginaBPlus noAtual = le_pagina_disco(arquivoAberto, posNoAtual, cabecalho);

    //  Usa para Nó atual e seus irmões para saber o minimo de chaves
    int minimoChaves = minChavesPagina(cabecalho, noAtual.ehfolha);
    // Como o pai é sempre página interna, então pode mandar como falso para folha.
    int minimoChavesPai = minChavesPagina(cabecalho, false);
    // Segundo - Encontra o index do Nó atual por meio dos ponteiros do pai, para
    // poder encontrar os irmaos
    int idxFilhoAtual = 0;
    while (idxFilhoAtual <= pai.qtdChaves &&
           *((long *)pai.ponteiros + idxFilhoAtual) != posNoAtual) {
        idxFilhoAtual++;
    }
    // Terceiro - Caso tenha irmãos a esquerda ou direita, é determinado a sua
    // posição (offsets)
    long posIrmaoEsq;
    if (idxFilhoAtual > 0) {
        // Se a condição for verdadeira, pegamos o ponteiro imediatamente anterior
        posIrmaoEsq = *((long *)pai.ponteiros + (idxFilhoAtual - 1));
    } else {
        // Se for falsa, não existe ninguém à esquerda
        posIrmaoEsq = Nulo;
    }

    long posIrmaoDir;
    if (idxFilhoAtual < pai.qtdChaves) {
        // Se a condição for verdadeira, pegamos o ponteiro imediatamente seguinte
        posIrmaoDir = *((long *)pai.ponteiros + (idxFilhoAtual + 1));
    } else {
        // Se for falsa, não existe ninguém à direita
        posIrmaoDir = Nulo;
    }
    // Variável para saber se deu para fazer só com redistribuição a esquerda ou a
    // direita resolve
    bool resolvido = false;

    // Tentativa de redistribuição a esquerda

    if (posIrmaoEsq != Nulo) {
        PaginaBPlus irmaoEsq = le_pagina_disco(arquivoAberto, posIrmaoEsq, cabecalho);

        // Verifica caso o irmão a esquerda tenha o suficiente para redistribuir
        if (irmaoEsq.qtdChaves > minimoChaves) {
            redistribuicao(&noAtual, &irmaoEsq, true, idxFilhoAtual, &pai, cabecalho);

            // Salva todas as páginas que foram modificadas usando redistribuição por
            // meio memória RAM dentro do disco
            escreve_pagina_disco(arquivoAberto, posNoAtual, &noAtual, cabecalho);
            escreve_pagina_disco(arquivoAberto, posIrmaoEsq, &irmaoEsq, cabecalho);
            escreve_pagina_disco(arquivoAberto, posPai, &pai, cabecalho);

            free(irmaoEsq.chaves);
            free(irmaoEsq.ponteiros);
            // Retorna como resolvido e não precisa continuar procurando soluções
            resolvido = true;
        } else {
            // se não tiver, só dá free nos ponteiros
            free(irmaoEsq.chaves);
            free(irmaoEsq.ponteiros);
        }
    }
    // Segunda tentativa, com a redistribuição a direita caso tenha falhado o da
    // esquerda.
    if (!resolvido && posIrmaoDir != Nulo) {
        PaginaBPlus irmaoDir = le_pagina_disco(arquivoAberto, posIrmaoDir, cabecalho);

        // Verifica caso o irmão a direita tenha o suficiente para redistribuir
        if (irmaoDir.qtdChaves > minimoChaves) {
            redistribuicao(&noAtual, &irmaoDir, false, idxFilhoAtual, &pai, cabecalho);

            // Salva todas as páginas modificadas de volta no disco
            escreve_pagina_disco(arquivoAberto, posNoAtual, &noAtual, cabecalho);
            escreve_pagina_disco(arquivoAberto, posIrmaoDir, &irmaoDir, cabecalho);
            escreve_pagina_disco(arquivoAberto, posPai, &pai, cabecalho);

            free(irmaoDir.chaves);
            free(irmaoDir.ponteiros);
            // retorna como resolvido e não precisa continuar procurando soluções
            resolvido = true;
        } else {
            free(irmaoDir.chaves);
            free(irmaoDir.ponteiros);
        }
    }

    // Caso todos os outros tenham falhados, realiza a fusão/concatenação:

    if (!resolvido) {
        // Por escolha, pegamos preferência por fundir com o irmão esquerdo se tiver
        if (posIrmaoEsq != Nulo) {

            PaginaBPlus irmaoEsq = le_pagina_disco(arquivoAberto, posIrmaoEsq, cabecalho);

            // A chave do pai que separa o Irmão Esq do nó Atual, ele pega o indice do
            // ponteiro da página irmão do atual.
            void *chavePai = (char *)pai.chaves + ((idxFilhoAtual - 1) * cabecalho->tamanhoChave);

            // Passamos irmaoEsq como 'atual' e nó Atual como 'irmao' para que o
            // conteúdo do irmão da esquerda seja concatenado com o nó atual
            concatenacao(arquivoAberto, &irmaoEsq, &noAtual, chavePai, cabecalho);

            // Grava o irmão esquerdo de volta no disco
            escreve_pagina_disco(arquivoAberto, posIrmaoEsq, &irmaoEsq, cabecalho);

            // Agora precisa remover a chave separadora e o ponteiro no PAI
            // Primeiro precisa encontrar o indice da chave para remover q é o indice
            int idxChaveRemover = idxFilhoAtual - 1;
            // Segundo precisa encontrar o números de elementos que precisa mover após
            // a remoção
            int numElementosMover = pai.qtdChaves - 1 - idxChaveRemover;
            // se for maior que 0, precisa fazer a deslocação
            if (numElementosMover > 0) {
                // Começa na chave imediatamente à direita da que vai ser retirada
                // (idxChaveRemover + 1) O local da chave que vai ser retirada
                // (idxChaveRemover) O memmove pega as chaves que precisam remover e
                // puxam para esquerda sobrepondo, por exemplo, o C0 foi tirado C0,C1,C2
                // -> C1,C2,C2,
                memmove((char *)pai.chaves + (idxChaveRemover * cabecalho->tamanhoChave),
                        (char *)pai.chaves + ((idxChaveRemover + 1) * cabecalho->tamanhoChave),
                        numElementosMover * cabecalho->tamanhoChave);

                // Faz a mesma coisa que a anterior, porém para os ponteiros e não as
                // chaves, P0,P1,P2,P3 -> P0,P1,P3,P3
                memmove((long *)pai.ponteiros + idxFilhoAtual,
                        (long *)pai.ponteiros + (idxFilhoAtual + 1),
                        numElementosMover * sizeof(long));
            }

            // Faz a remoção de vez, aquele C3 a mais não é mais contabilizado.
            pai.qtdChaves--;

            // O nó Atual virou lixo e precisa colocar ela na lista de nós livres para
            // ser reutilizado Usando o proximaFolha para guardar o Nó livre anterior
            // que foi apagada ou nulo dentro desse Nó removido
            noAtual.proximaFolha = cabecalho->proxNolivre;
            // escreve isso no disco nessa parte do noAtual
            escreve_pagina_disco(arquivoAberto, posNoAtual, &noAtual, cabecalho);

            // O cabeçalho agora aponta para o noAtual recém-removida como o primeiro
            // espaço livre
            cabecalho->proxNolivre = posNoAtual;

            // Salva o pai alterado e o cabeçalho no disco
            escreve_pagina_disco(arquivoAberto, posPai, &pai, cabecalho);
            fseek(arquivoAberto, 0, SEEK_SET);
            fwrite(cabecalho, sizeof(CabecalhoBPlus), 1, arquivoAberto);

            free(irmaoEsq.chaves);
            free(irmaoEsq.ponteiros);
        }

        // Se não tem irmão esquerdo, funde com o direito
        else if (posIrmaoDir != Nulo) {

            PaginaBPlus irmaoDir = le_pagina_disco(arquivoAberto, posIrmaoDir, cabecalho);

            // A chave do pai que separa o nó Atual do Irmão Dir fica no índice
            // (idxFilhoAtual)
            void *chavePai = (char *)pai.chaves + (idxFilhoAtual * cabecalho->tamanhoChave);

            // Chamamos a sua função concatenacao. O nó atual concatena com o irmão
            // direito.
            concatenacao(arquivoAberto, &noAtual, &irmaoDir, chavePai, cabecalho);

            // Grava o nó de volta no disco
            escreve_pagina_disco(arquivoAberto, posNoAtual, &noAtual, cabecalho);

            // Mesmo processo que foi feito na parte do irmão esquerdo de remover a
            // chave separadora e o ponteiro no PAI, porém agora para o irmão direito
            //  Primeira diferença é que utiliza o indíce da página atual invés do
            //  esquerdo.
            int idxChaveRemover = idxFilhoAtual;

            // Números de chaves que vai precisar mover
            int numElementosMover = pai.qtdChaves - 1 - idxChaveRemover;

            // se for maior que 0
            if (numElementosMover > 0) {

                // Puxa as chaves do pai para a esquerda, assim como feito no caso
                // anterior com o irmão esquerdo
                memmove((char *)pai.chaves + (idxChaveRemover * cabecalho->tamanhoChave),
                        (char *)pai.chaves + ((idxChaveRemover + 1) * cabecalho->tamanhoChave),
                        numElementosMover * cabecalho->tamanhoChave);

                // Puxa os ponteiros do pai para a esquerda como o caso anterior com o
                // irmão esquerdo; A diferença é que agora o ponteiro a ser sobrescrito
                // é o do irmaoDir e não do o do atual.
                memmove((long *)pai.ponteiros + (idxFilhoAtual + 1),
                        (long *)pai.ponteiros + (idxFilhoAtual + 2),
                        numElementosMover * sizeof(long));
            }
            // Termina a remoção de vez chave na parte do pai.
            pai.qtdChaves--;

            // O No atual virou lixo e precisa colocar ela na lista de nós livres para
            // ser reutilizado

            // Mesma coisa que o caso anterior anterior porém para o irmão a direita,
            // Primeiro colocamos que a proxima folha do irmao dir vai apontar para o
            // nolivre anterior apagado
            irmaoDir.proximaFolha = cabecalho->proxNolivre;

            // Escreve no disco isso.
            escreve_pagina_disco(arquivoAberto, posIrmaoDir, &irmaoDir, cabecalho);

            // O cabeçalho agora aponta para esse irmão morto como o primeiro espaço
            // livre
            cabecalho->proxNolivre = posIrmaoDir;

            // Salva as alterações do pai
            escreve_pagina_disco(arquivoAberto, posPai, &pai, cabecalho);

            // Volta o ponteiro do arquivo para o Cabeçalho
            fseek(arquivoAberto, 0, SEEK_SET);
            // Salva o novo cabeçalho com as informações novas do cabeçalho como a
            // mudança no proxNoLivre
            fwrite(cabecalho, sizeof(CabecalhoBPlus), 1, arquivoAberto);

            // da free nos ponteiros/vetores que não vão ser mais usados
            free(irmaoDir.chaves);
            free(irmaoDir.ponteiros);
        }
        // se o pai tiver ficado desbalanceado, faz recursão
        if (pai.qtdChaves < minimoChavesPai) {
            BalancearArvoreUnderflow(arquivoAberto, cabecalho, posPai, caminho, tam_caminho - 1);
        }
        // se estiver balanceado, ele coloca como resolvido.
        resolvido = true;
    }

    // limpeza dos ponteiros e vetores usados finais na ram.
    free(pai.chaves);
    free(pai.ponteiros);
    free(noAtual.chaves);
    free(noAtual.ponteiros);
}

void imprimePagina(PaginaBPlus pagina, CabecalhoBPlus *cabecalho,
                   void (*imprimeChave)(void *chave)) {
    printf("[");
    for (int i = 0; i < pagina.qtdChaves; i++) {
        // Aritmética correta baseada no tamanho configurado da chave
        void *chaveAtual = (char *)pagina.chaves + (i * cabecalho->tamanhoChave);

        imprimeChave(chaveAtual);

        if (pagina.ehfolha) {
            printf("*");
        }
        if (i < pagina.qtdChaves - 1) {
            printf(", ");
        }
    }
    printf("]");
}

void imprimeBPlus(FILE *arquivo, CabecalhoBPlus *cabecalho, long posisaoPagina, const char *prefixo,
                  bool ehUltimo, void (*imprimeChave)(void *chave)) {

    if (posisaoPagina == Nulo) {
        return;
    }

    PaginaBPlus pagina = le_pagina_disco(arquivo, posisaoPagina, cabecalho);

    printf("%s", prefixo);

    if (ehUltimo) {
        printf("└── ");
    } else {
        printf("├── ");
    }

    imprimePagina(pagina, cabecalho, imprimeChave);
    printf("\n");

    if (!pagina.ehfolha) {
        for (int i = 0; i <= pagina.qtdChaves; i++) {
            char novoPrefixo[256];
            bool filhoEhUltimo = (i == pagina.qtdChaves);

            if (ehUltimo) {
                snprintf(novoPrefixo, sizeof(novoPrefixo), "%s    ", prefixo);
            } else {
                snprintf(novoPrefixo, sizeof(novoPrefixo), "%s│   ", prefixo);
            }
            long posFilho = *((long *)((char *)pagina.ponteiros + (i * sizeof(long))));

            // Só tenta imprimir o filho se o ponteiro for válido (diferente de Nulo/-1)
            if (posFilho != Nulo) {
                imprimeBPlus(arquivo, cabecalho, posFilho, novoPrefixo, filhoEhUltimo,
                             imprimeChave);
            }
        }
    }

    // Limpeza dos auxiliares alocados no le_pagina_disco
    if (pagina.chaves)
        free(pagina.chaves);
    if (pagina.ponteiros)
        free(pagina.ponteiros);
}