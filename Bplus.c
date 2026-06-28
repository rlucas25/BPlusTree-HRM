#include "Bplus.h"
#include <cstring>

/*
typedef struct{
    long posRaiz;
    long ordem;
    size_t tamanhoChave;
    size_t tamanhoRegistro;
    long proxNolivre;
}CabecalhoBPlus;
*/

bool criaArvore(const char *nomeArquivo, int ordem, size_t tamanhoChave, size_t tamanhoRegistro){
    if ( !nomeArquivo && !tamanhoChave && !tamanhoRegistro ){
        erro("Parametros invalidos passados para criaArvore"); 
        return false;
    }
    
    FILE *arquivo = fopen(nomeArquivo,"wb");

    if ( !arquivo ){
        erro("Não foi possivel abrir arquivo em criaArvore");
        return false;
    }
    
    CabecalhoBPlus cabecalho;
    cabecalho.posRaiz = Nulo;
    cabecalho.ordem = ordem;
    cabecalho.tamanhoChave = tamanhoChave;
    cabecalho.tamanhoRegistro = tamanhoRegistro;
    cabecalho.proxNolivre = Nulo;

    fseek(arquivo,0,SEEK_SET);
    size_t gravados = fwrite(&cabecalho, sizeof(CabecalhoBPlus), 1, arquivo);
    fclose(arquivo);
    return (gravados > 0);
}

FILE *abrirArvore(const char *nomeArquivo, CabecalhoBPlus *cabecalho){
    
    if ( !nomeArquivo || !cabecalho){
        erro("Paremetro invalidos passados para abrirArvore");
        return NULL; 
    }
    //nome do ponteiro vc decide ai
    FILE* arquivo = fopen(nomeArquivo,"rb");

    if ( !arquivo ){
        erro("Não foi possivel abrir arquivo em abrirArvore");
        return NULL;
    }
    fseek(arquivo,0,SEEK_SET);
    fread(cabecalho, sizeof(CabecalhoBPlus), 1, arquivo);

    return arquivo;
        
}

void fecharArvore(FILE *arquivoAberto){
    if( !arquivoAberto ){
        erro("Arquivo nulo ou ja fechado em fecharArvore");
    }
    
    fclose(arquivoAberto);
}
//lendo a página para não integrar a os ponteiros void na leitura do read dele.
PaginaBPlus le_pagina_disco(FILE *arquivo, long offset, CabecalhoBPlus *cabecalho) {
    PaginaBPlus pagina;
    fseek(arquivo,offset,SEEK_SET);
    fread(&pagina.qtdChaves, sizeof(int), 1, arquivo);
    fread(&pagina.ehfolha, sizeof(bool), 1, arquivo);
    fread(&pagina.proximaFolha, sizeof(long), 1, arquivo);
    
    pagina.chaves = calloc(cabecalho->ordem, cabecalho->tamanhoChave);
    if (pagina.ehfolha){
        pagina.ponteiros = calloc(cabecalho->ordem, cabecalho->tamanhoRegistro);
    } else {
        pagina.ponteiros = calloc(cabecalho->ordem + 1, sizeof(long));
    }

    fread(pagina.chaves, cabecalho->tamanhoChave, cabecalho->ordem, arquivo);

    if (pagina.ehfolha) {
        fread(pagina.ponteiros, cabecalho->tamanhoRegistro, cabecalho->ordem, arquivo);
    } else {
        fread(pagina.ponteiros, sizeof(long), cabecalho->ordem + 1, arquivo);
    }

    return pagina;
}

bool escreve_pagina_disco(FILE *arquivo, long offset, PaginaBPlus *pagina, CabecalhoBPlus *cabecalho) {
    fseek(arquivo, offset, SEEK_SET);
    fwrite(&pagina->qtdChaves, sizeof(int), 1, arquivo);
    fwrite(&pagina->ehfolha, sizeof(bool), 1, arquivo);
    fwrite(&pagina->proximaFolha, sizeof(long), 1, arquivo);

    fwrite(pagina->chaves, cabecalho->tamanhoChave, cabecalho->ordem, arquivo);

    if (pagina->ehfolha) {
        fwrite(pagina->ponteiros, cabecalho->tamanhoRegistro, cabecalho->ordem, arquivo);
    } else {
        fwrite(pagina->ponteiros, sizeof(long), cabecalho->ordem + 1, arquivo);
    }

    return true;
    
}

long buscarChave(FILE *arquivoAberto,bool *encontrou, CabecalhoBPlus *cabecalho, const void *chave, int comparaChaves(const void *x,const void *y),long *caminho, int *tam_caminho ){
    
    if (!arquivoAberto || !cabecalho || !chave || !comparaChaves || !encontrou){
        erro("Paremetro invalidos passados para buscarChave");
        return -1; 
    }

    long posicaoAtual  = cabecalho->posRaiz;
    int i;
    *encontrou = false;
    *tam_caminho = 0; // Começamos com o caminho vazio
    
    if (posicaoAtual == Nulo){

        return 0;

    }
  
    while (posicaoAtual != Nulo){

        PaginaBPlus paginaAtual = le_pagina_disco(arquivoAberto, posicaoAtual, cabecalho);

        i = 0;
        
        while (i < paginaAtual.qtdChaves && comparaChaves(chave, (char*)paginaAtual.chaves + (i * cabecalho->tamanhoChave)) < 0){

            i++;

        }
        
        if (paginaAtual.ehfolha){

            if (i < paginaAtual.qtdChaves && comparaChaves(chave, (char*)paginaAtual.chaves + (i * cabecalho->tamanhoChave)) == 0){

                *encontrou = true;

            }else{

                *encontrou = false;
            }

            free(paginaAtual.chaves);
            free(paginaAtual.ponteiros);
            return posicaoAtual;

        }
        
        // =========================================================
        // O RASTRO DE MIGALHAS: Anotamos onde estamos antes de descer
        // =========================================================
        if (caminho != NULL && tam_caminho != NULL) {
            caminho[*tam_caminho] = posicaoAtual;
            (*tam_caminho)++;
        }
        
        posicaoAtual = *( (long*)paginaAtual.ponteiros + i );
        free(paginaAtual.chaves);
        free(paginaAtual.ponteiros);  
    }

    return -1;
    
}

PaginaBPlus *cisaoPagina(PaginaBPlus *pagina, PaginaBPlus *paginaPai, CabecalhoBPlus *cabecalho, void* chavePromovida){

    PaginaBPlus *novaPagina;
    void *meio;
    int i;
    int j;

    if ( !pagina || !cabecalho  || !chavePromovida){
        erro("Parametros invalidos passados para insereChave");
        return NULL;
    }
    
    novaPagina = mallocSafe(sizeof(PaginaBPlus));

    novaPagina->ehfolha = pagina->ehfolha;
    novaPagina->qtdChaves = 0;
    novaPagina->proximaFolha = Nulo;
    
    meio = (char*)pagina->chaves + (((cabecalho->ordem)) * cabecalho->tamanhoChave);
    
    //paginaPai[paginaPai->qntdChaves] = pagina[meio];
    //novapagina [i] = pagina[meio+1] até o maxmo de elementos da pagina
    
    //void temp[cabecalho->tamanhoChave];
    
    memcpy((char*)paginaPai->chaves + (paginaPai->qtdChaves * cabecalho->tamanhoChave), meio, cabecalho->tamanhoChave);
    paginaPai->qtdChaves++;

    for (int i = 1; i <= cabecalho->ordem; i++){
        memcpy(novaPagina->chaves + i - 1, meio + i, cabecalho->tamanhoChave);
        novaPagina->qtdChaves++;
        pagina->qtdChaves--;
    }
    // MANUTENCAO
    memcpy((long*)novaPagina->ponteiros + novaPagina->qtdChaves, cabecalho->proxNolivre,sizeof(long)); 
    // MANUTENCAO
    // MANUTENCAO
    // MANUTENCAO
}

//precisa terminar, vou terminar depois
void fusao (FILE *arquivo, PaginaBPlus *atual, PaginaBPlus *irmao, CabecalhoBPlus *cabecalho){
    
    //copiar a chave do irmao
    void *destino = (char*)atual->chaves + (atual->qtdChaves * cabecalho->tamanhoChave);
    
    //se não é folha tem que descer o pai no meio
    if (atual->ehfolha != false && chavePai != NULL){
        memcpy (destino, chavePai, cabecalho->tamanhoChave);
        destino = (char*)destino + cabecalho->tamanhoChave;        
    }
    memcpy(destino, irmao->chaves, irmao->qtdChaves * cabecalho->tamanhoChave);
    

    if (atual->ehfolha){
        void *destinoRegistro  = (char*)atual->ponteiros + (atual->qtdChaves * cabecalho->tamanhoRegistro);
        memcpy(destinoRegistro, irmao->ponteiros, irmao->qtdChaves * cabecalho->tamanhoRegistro);
    } else {
        
    }

}

bool insereChave(FILE *arquivoAberto, CabecalhoBPlus *cabecalho, const void *chave, const void *registro, void comparaChaves(void *x,void *y)){
    int (*compararChaves)(const void*, const void*) = (int (*)(const void*, const void*))comparaChaves;
    if ( !arquivoAberto || !cabecalho || !chave || !registro || !comparaChaves){
        erro("Parametros invalidos passados para insereChave");
        return false;
    }
    
    PaginaBPlus paginaNova;
    // se arvore estiver vazia
    if (cabecalho->posRaiz == Nulo){
        paginaNova.ehfolha = true;
        paginaNova.qtdChaves = 1;
        paginaNova.proximaFolha = Nulo;
        paginaNova.chaves = mallocSafe(cabecalho->ordem * cabecalho->tamanhoChave);
        paginaNova.ponteiros = mallocSafe((cabecalho->ordem + 1) * cabecalho->tamanhoRegistro);
            
        
        memcpy(paginaNova.chaves, chave, cabecalho->tamanhoChave);
        memcpy(paginaNova.ponteiros, registro, cabecalho->tamanhoRegistro);
       
        cabecalho->posRaiz = sizeof(CabecalhoBPlus);
        fseek(arquivoAberto, cabecalho->posRaiz, SEEK_SET);
        fwrite(&paginaNova.qtdChaves, sizeof(long), 1, arquivoAberto);
        fwrite(&paginaNova.ehfolha, sizeof(bool),1, arquivoAberto);
        fwrite(&paginaNova.proximaFolha, sizeof(bool),1, arquivoAberto);

        //colocando o cabeçalho novo.
        fseek(arquivoAberto, 0, SEEK_SET);
        fwrite(cabecalho, sizeof(CabecalhoBPlus),1,arquivoAberto);
        
        free(paginaNova.chaves);
        free(paginaNova.ponteiros);

        return true;
    }

    // arvore ja tem chaves
    bool encontrou = false;
    long posFolha = buscarChave(arquivoAberto, &encontrou, cabecalho, chave, compararChaves);

    if (*encontrou){
        erro("Chave duplicada na arvore");  
        return false;
    }

    PaginaBPlus folha;
    // faz de traz pra frente pra facilitar o momento de empurrar as outras chaves na memoria
    int i = folha.qtdChaves -  1;
    while( i >= 0 ) {
        // pega a chave a frente
        void *chaveAtual = folha.chaves + (i * cabecalho->tamanhoChave);
        if (compararChaves(chave, chaveAtual) < 0){
            memcpy((char*)folha.chaves + ((i + 1) *cabecalho->tamanhoChave), (char*)chaveAtual, cabecalho->tamanhoChave);
            memcpy((char*)folha.ponteiros + ((i + 1) *cabecalho->tamanhoRegistro), (char*)folha.ponteiros, cabecalho->tamanhoRegistro);
            i--;
        }
        else {
            break;
        }
        
    }
    memcpy((char*)folha.chaves + ((i + 1) *cabecalho->tamanhoChave), (char*)chave, cabecalho->tamanhoChave);
    memcpy((char*)folha.ponteiros + ((i + 1) *cabecalho->tamanhoRegistro), (char*)registro, cabecalho->tamanhoRegistro);
    folha.qtdChaves++;
    
    if(folha.qtdChaves > cabecalho->ordem){
        //estourou a capacidade, fazer a cisao
        // funcao de cisao
    }
    else{
        //fazer funcao de salvar a pagina no disco
    }
    
    free(folha.ponteiros);
    free(folha.chaves);
    return true;
}



bool removeChave(FILE *arquivoAberto, CabecalhoBPlus *cabecalho, const void *chave, void comparaChaves(void *x,void *y)){

    

}