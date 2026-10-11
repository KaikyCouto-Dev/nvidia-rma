#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void utils_limpar_tela(void)
{
    (void)system("clear");
}

void utils_pausar(void)
{
    char linha[TAM_LINHA];
    printf("\nPressione ENTER para continuar...");
    fflush(stdout);
    if (!fgets(linha, sizeof linha, stdin)) {
        clearerr(stdin);
    }
}

void utils_separador(void)
{
    printf("------------------------------------------------------------\n");
}

void utils_titulo(const char *texto)
{
    printf("\n");
    utils_separador();
    printf(" %s\n", texto);
    utils_separador();
}

void utils_trim(char *s)
{
    size_t ini = 0, fim;
    if (!s) {
        return;
    }
    while (s[ini] != '\0' && isspace((unsigned char)s[ini])) {
        ini++;
    }
    if (s[ini] == '\0') {
        s[0] = '\0';
        return;
    }
    fim = strlen(s);
    while (fim > ini && isspace((unsigned char)s[fim - 1])) {
        fim--;
    }
    if (ini > 0) {
        memmove(s, s + ini, fim - ini);
    }
    s[fim - ini] = '\0';
}

int utils_vazio(const char *s)
{
    return s == NULL || s[0] == '\0';
}

int utils_ler_linha(char *destino, size_t tamanho)
{
    if (!fgets(destino, (int)tamanho, stdin)) {
        clearerr(stdin);
        destino[0] = '\0';
        return 0;
    }
    destino[strcspn(destino, "\r\n")] = '\0';
    utils_trim(destino);
    return 1;
}

int utils_ler_int(const char *prompt, int *destino)
{
    char linha[TAM_LINHA];
    char extra;
    int valor;

    printf("%s", prompt);
    fflush(stdout);
    if (!utils_ler_linha(linha, sizeof linha)) {
        return 0;
    }
    if (linha[0] == '\0') {
        return 0;
    }
    if (sscanf(linha, "%d%c", &valor, &extra) != 1) {
        printf("Valor inteiro invalido.\n");
        return 0;
    }
    *destino = valor;
    return 1;
}

int utils_ler_int_faixa(const char *prompt, int *destino, int min, int max)
{
    if (!utils_ler_int(prompt, destino)) {
        return 0;
    }
    if (*destino < min || *destino > max) {
        printf("Informe um numero entre %d e %d.\n", min, max);
        return 0;
    }
    return 1;
}

int utils_ler_double(const char *prompt, double *destino)
{
    char linha[TAM_LINHA];
    char extra;
    double valor;

    printf("%s", prompt);
    fflush(stdout);
    if (!utils_ler_linha(linha, sizeof linha)) {
        return 0;
    }
    if (sscanf(linha, "%lf%c", &valor, &extra) != 1) {
        printf("Valor numerico invalido.\n");
        return 0;
    }
    *destino = valor;
    return 1;
}

int utils_ler_string(const char *prompt, char *destino, size_t tamanho, int obrigatorio)
{
    printf("%s", prompt);
    fflush(stdout);
    if (!utils_ler_linha(destino, tamanho)) {
        return 0;
    }
    if (obrigatorio && utils_vazio(destino)) {
        printf("Este campo e obrigatorio.\n");
        return 0;
    }
    return 1;
}

int utils_confirmar(const char *prompt)
{
    char linha[TAM_LINHA];
    printf("%s (s/n): ", prompt);
    fflush(stdout);
    if (!utils_ler_linha(linha, sizeof linha)) {
        return 0;
    }
    return linha[0] == 's' || linha[0] == 'S';
}

int utils_validar_email(const char *email)
{
    const char *arroba;
    const char *ponto;

    if (utils_vazio(email)) {
        return 0;
    }
    arroba = strchr(email, '@');
    if (!arroba || arroba == email || *(arroba + 1) == '\0') {
        return 0;
    }
    if (strchr(arroba + 1, '@')) {
        return 0;
    }
    ponto = strchr(arroba + 1, '.');
    if (!ponto || *(ponto + 1) == '\0') {
        return 0;
    }
    return 1;
}

static int ano_bissexto(int ano)
{
    return (ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0);
}

static int dias_no_mes(int ano, int mes)
{
    static const int dias[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (mes == 2 && ano_bissexto(ano)) {
        return 29;
    }
    if (mes < 1 || mes > 12) {
        return 0;
    }
    return dias[mes];
}

int utils_validar_data(const char *data)
{
    int ano, mes, dia;
    char extra;

    if (utils_vazio(data) || strlen(data) != 10) {
        return 0;
    }
    if (data[4] != '-' || data[7] != '-') {
        return 0;
    }
    if (sscanf(data, "%d-%d-%d%c", &ano, &mes, &dia, &extra) != 3) {
        return 0;
    }
    if (ano < 1990 || ano > 2100 || mes < 1 || mes > 12) {
        return 0;
    }
    if (dia < 1 || dia > dias_no_mes(ano, mes)) {
        return 0;
    }
    return 1;
}

void utils_data_hoje(char *destino, size_t tamanho)
{
    time_t agora = time(NULL);
    struct tm *tm_info = localtime(&agora);
    if (!tm_info) {
        snprintf(destino, tamanho, "1970-01-01");
        return;
    }
    snprintf(destino, tamanho, "%04d-%02d-%02d",
             tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday);
}

int utils_data_add_meses(const char *data, int meses, char *destino, size_t tamanho)
{
    int ano, mes, dia;
    int total;

    if (!utils_validar_data(data)) {
        return 0;
    }
    sscanf(data, "%d-%d-%d", &ano, &mes, &dia);
    total = ano * 12 + (mes - 1) + meses;
    ano = total / 12;
    mes = (total % 12) + 1;
    if (dia > dias_no_mes(ano, mes)) {
        dia = dias_no_mes(ano, mes);
    }
    snprintf(destino, tamanho, "%04d-%02d-%02d", ano, mes, dia);
    return 1;
}

int utils_data_cmp(const char *a, const char *b)
{
    return strcmp(a, b);
}

int utils_dentro_garantia(const char *data_compra, int meses_garantia)
{
    char vencimento[TAM_DATA];
    char hoje[TAM_DATA];

    if (!utils_data_add_meses(data_compra, meses_garantia, vencimento, sizeof vencimento)) {
        return 0;
    }
    utils_data_hoje(hoje, sizeof hoje);
    /* garantia vigente se hoje <= data de vencimento */
    return utils_data_cmp(hoje, vencimento) <= 0;
}

int utils_status_rma_valido(const char *status)
{
    static const char *lista[] = {
        "ABERTO", "EM_ANALISE", "APROVADO", "REJEITADO",
        "AGUARDANDO_PECA", "ENVIADO", "CONCLUIDO", "CANCELADO", NULL
    };
    int i;
    if (!status) {
        return 0;
    }
    for (i = 0; lista[i]; i++) {
        if (strcmp(status, lista[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int utils_prioridade_valida(const char *prio)
{
    return prio && (strcmp(prio, "BAIXA") == 0 || strcmp(prio, "MEDIA") == 0 ||
                    strcmp(prio, "ALTA") == 0 || strcmp(prio, "CRITICA") == 0);
}

int utils_status_serie_valido(const char *status)
{
    return status && (strcmp(status, "DISPONIVEL") == 0 || strcmp(status, "VENDIDO") == 0 ||
                      strcmp(status, "EM_RMA") == 0 || strcmp(status, "SUBSTITUIDO") == 0 ||
                      strcmp(status, "DESCARTADO") == 0);
}

int utils_rma_status_aberto(const char *status)
{
    if (!status) {
        return 0;
    }
    return strcmp(status, "CONCLUIDO") != 0 &&
           strcmp(status, "REJEITADO") != 0 &&
           strcmp(status, "CANCELADO") != 0;
}

void utils_menu_status_rma(void)
{
    printf("Status: ABERTO | EM_ANALISE | APROVADO | REJEITADO |\n");
    printf("        AGUARDANDO_PECA | ENVIADO | CONCLUIDO | CANCELADO\n");
}

void utils_menu_prioridade(void)
{
    printf("Prioridade: BAIXA | MEDIA | ALTA | CRITICA\n");
}

void utils_menu_status_serie(void)
{
    printf("Status: DISPONIVEL | VENDIDO | EM_RMA | SUBSTITUIDO | DESCARTADO\n");
}
