#ifndef CLIENTE_H
#define CLIENTE_H

#include "utils.h"

typedef struct {
    int  id;
    char nome[TAM_NOME];
    char empresa[TAM_EMPRESA];
    char email[TAM_EMAIL];
    char telefone[TAM_FONE];
    char cidade[TAM_CIDADE];
    char estado[TAM_ESTADO];
    char data_cadastro[TAM_DATA];
    int  ativo;
} Cliente;

void cliente_menu(void);
int  cliente_existe_ativo(int id);
void cliente_exibir_por_id(int id);

#endif
