// Turma: 3704.1
// Grupo 14
// DENNIS FRANCISCO GUIMARÃES DE OLIVEIRA BARACHO; 2024200496
// HEITOR SANTOS MAIA; 2024200498
// LUCAS REBOUÇAS ALMEIDA; 2024200501
// MARCOS VINÍCIUS PIMENTEL GOMES; 2024200500

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>


/**
 * @brief Exibe mensagem de erro personalizada para usuario
 * 
 * @details A mensagem é exibida em uma tonalidade de vermelho
 * @param mensagem Ponteiro para mensagem que sera exibida
 */
void erro(const char *mensagem);


/**
 * @brief Exibe mensagem de êxito (sucesso) personalizada para usuario
 * 
 * @details A mensagem é exibida em uma tonalidade de Verde
 * @param mensagem Ponteiro para mensagem que sera exibida
 */
void exito(const char *mensagem);


/**
 * @brief Realiza a alocação e verifica se a alocação foi 
 *        feita corretamente
 * 
 * @param nbytes quantidade de bytes que serão alocados no proecesso 
 */
void *mallocSafe(size_t nbytes);
