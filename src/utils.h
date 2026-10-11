#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

#define TAM_NOME      128
#define TAM_EMAIL     160
#define TAM_FONE      40
#define TAM_CIDADE    80
#define TAM_ESTADO    8
#define TAM_DATA      16
#define TAM_MODELO    96
#define TAM_CATEGORIA 80
#define TAM_SERIAL    64
#define TAM_STATUS    32
#define TAM_TEXTO     1024
#define TAM_LINHA     1024
#define TAM_EMPRESA   128

void utils_limpar_tela(void);
void utils_pausar(void);
void utils_separador(void);
void utils_titulo(const char *texto);

/* Lê uma linha, remove \n e espaços nas extremidades. Retorna 0 em EOF. */
int utils_ler_linha(char *destino, size_t tamanho);

int utils_ler_int(const char *prompt, int *destino);
int utils_ler_int_faixa(const char *prompt, int *destino, int min, int max);
int utils_ler_double(const char *prompt, double *destino);

/* obrigatorio != 0 rejeita string vazia. */
int utils_ler_string(const char *prompt, char *destino, size_t tamanho, int obrigatorio);

int  utils_confirmar(const char *prompt);
void utils_trim(char *s);
int  utils_vazio(const char *s);
int  utils_validar_email(const char *email);
int  utils_validar_data(const char *data); /* AAAA-MM-DD */
void utils_data_hoje(char *destino, size_t tamanho);
int  utils_data_add_meses(const char *data, int meses, char *destino, size_t tamanho);
int  utils_data_cmp(const char *a, const char *b);
int  utils_dentro_garantia(const char *data_compra, int meses_garantia);

int utils_status_rma_valido(const char *status);
int utils_prioridade_valida(const char *prio);
int utils_status_serie_valido(const char *status);
int utils_rma_status_aberto(const char *status);

void utils_menu_status_rma(void);
void utils_menu_prioridade(void);
void utils_menu_status_serie(void);

#endif
