#ifndef RMA_H
#define RMA_H

#include "utils.h"

typedef struct {
    int  id;
    int  cliente_id;
    int  estoque_serie_id;
    char data_abertura[TAM_DATA];
    char descricao_problema[TAM_TEXTO];
    char status[TAM_STATUS];
    char prioridade[TAM_STATUS];
    char data_conclusao[TAM_DATA];
    char observacoes[TAM_TEXTO];
} Rma;

void rma_menu(void);

#endif
