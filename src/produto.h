#ifndef PRODUTO_H
#define PRODUTO_H

#include "utils.h"

typedef struct {
    int    id;
    char   nome[TAM_NOME];
    char   categoria[TAM_CATEGORIA];
    char   modelo[TAM_MODELO];
    double preco;
    int    meses_garantia;
    int    ativo;
} Produto;

void produto_menu(void);
int  produto_existe_ativo(int id);
void produto_exibir_por_id(int id);

#endif
