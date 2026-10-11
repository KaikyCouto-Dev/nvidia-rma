#ifndef ESTOQUE_H
#define ESTOQUE_H

#include <stddef.h>

void estoque_menu_series(void);
void estoque_menu_pecas(void);
void estoque_menu_venda(void);

int  estoque_serie_por_numero(const char *numero, int *id, int *produto_id, char *status, size_t status_tam);
int  estoque_atualizar_status_serie(int serie_id, const char *status);
int  estoque_cliente_da_serie(int serie_id, int *cliente_id, char *data_compra, size_t tam_data);
int  peca_baixar_estoque(int peca_id, int quantidade);
void peca_alertar_minimos(void);

#endif
