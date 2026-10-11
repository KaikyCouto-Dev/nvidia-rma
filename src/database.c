#include "database.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

static sqlite3 *g_db = NULL;

static char *ler_arquivo_texto(const char *caminho)
{
    FILE *f;
    long tamanho;
    char *buf;

    f = fopen(caminho, "rb");
    if (!f) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return NULL;
    }
    tamanho = ftell(f);
    if (tamanho < 0) {
        fclose(f);
        return NULL;
    }
    rewind(f);
    buf = malloc((size_t)tamanho + 1);
    if (!buf) {
        fclose(f);
        return NULL;
    }
    if (fread(buf, 1, (size_t)tamanho, f) != (size_t)tamanho) {
        free(buf);
        fclose(f);
        return NULL;
    }
    buf[tamanho] = '\0';
    fclose(f);
    return buf;
}

static int garantir_pasta_db(const char *caminho)
{
    char pasta[512];
    const char *barra;
    size_t n;

    barra = strrchr(caminho, '/');
    if (!barra) {
        return 1;
    }
    n = (size_t)(barra - caminho);
    if (n >= sizeof pasta) {
        return 0;
    }
    memcpy(pasta, caminho, n);
    pasta[n] = '\0';
    if (mkdir(pasta, 0755) != 0) {
        struct stat st;
        if (stat(pasta, &st) != 0 || !S_ISDIR(st.st_mode)) {
            perror("mkdir");
            return 0;
        }
    }
    return 1;
}

int database_abrir(const char *caminho)
{
    int rc;

    if (!garantir_pasta_db(caminho)) {
        return 0;
    }
    rc = sqlite3_open(caminho, &g_db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Nao foi possivel abrir o banco '%s': %s\n",
                caminho, g_db ? sqlite3_errmsg(g_db) : sqlite3_errstr(rc));
        if (g_db) {
            sqlite3_close(g_db);
            g_db = NULL;
        }
        return 0;
    }
    sqlite3_busy_timeout(g_db, 3000);
    database_executar("PRAGMA foreign_keys = ON;");
    return 1;
}

void database_fechar(void)
{
    if (g_db) {
        sqlite3_close(g_db);
        g_db = NULL;
    }
}

sqlite3 *database_conexao(void)
{
    return g_db;
}

void database_print_erro(const char *contexto)
{
    fprintf(stderr, "%s: %s\n", contexto, g_db ? sqlite3_errmsg(g_db) : "banco nao aberto");
}

int database_executar(const char *sql)
{
    char *erro = NULL;
    int rc;

    if (!g_db) {
        fprintf(stderr, "Banco de dados nao esta aberto.\n");
        return 0;
    }
    rc = sqlite3_exec(g_db, sql, NULL, NULL, &erro);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erro ao executar SQL: %s\n", erro ? erro : sqlite3_errmsg(g_db));
        sqlite3_free(erro);
        return 0;
    }
    return 1;
}

int database_preparar(sqlite3_stmt **stmt, const char *sql)
{
    int rc = sqlite3_prepare_v2(g_db, sql, -1, stmt, NULL);
    if (rc != SQLITE_OK) {
        database_print_erro("Falha ao preparar consulta");
        *stmt = NULL;
        return 0;
    }
    return 1;
}

long long database_ultimo_id(void)
{
    return g_db ? sqlite3_last_insert_rowid(g_db) : 0;
}

int database_contar(const char *sql)
{
    sqlite3_stmt *stmt = NULL;
    int n = 0;

    if (!database_preparar(&stmt, sql)) {
        return -1;
    }
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        n = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return n;
}

int database_imprimir_consulta(const char *sql)
{
    sqlite3_stmt *stmt = NULL;
    int ncols, i, linhas = 0;

    if (!database_preparar(&stmt, sql)) {
        return 0;
    }
    ncols = sqlite3_column_count(stmt);
    for (i = 0; i < ncols; i++) {
        printf("%-24.24s", sqlite3_column_name(stmt, i));
    }
    printf("\n");
    for (i = 0; i < ncols; i++) {
        printf("------------------------");
    }
    printf("\n");

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        linhas++;
        for (i = 0; i < ncols; i++) {
            const unsigned char *txt = sqlite3_column_text(stmt, i);
            printf("%-24.24s", txt ? (const char *)txt : "(nulo)");
        }
        printf("\n");
    }
    sqlite3_finalize(stmt);
    if (linhas == 0) {
        printf("(nenhum registro encontrado)\n");
    } else {
        printf("\nTotal: %d linha(s).\n", linhas);
    }
    return 1;
}

int database_inicializar(void)
{
    char *schema;
    int ok;

    schema = ler_arquivo_texto("sql/schema.sql");
    if (!schema) {
        fprintf(stderr,
                "Arquivo sql/schema.sql nao encontrado.\n"
                "Execute o programa a partir da raiz do projeto (nvidia-rma/).\n");
        return 0;
    }
    ok = database_executar(schema);
    free(schema);
    return ok;
}

int database_popular_teste(void)
{
    char *seed;
    int clientes;
    int ok;

    clientes = database_contar("SELECT COUNT(*) FROM clientes;");
    if (clientes < 0) {
        return 0;
    }
    if (clientes > 0) {
        printf("O banco ja possui %d cliente(s).\n", clientes);
        if (!utils_confirmar("Inserir dados de teste mesmo assim? (pode falhar por e-mails/seriais duplicados)")) {
            printf("Operacao cancelada.\n");
            return 1;
        }
    }

    seed = ler_arquivo_texto("sql/seed.sql");
    if (!seed) {
        fprintf(stderr, "Arquivo sql/seed.sql nao encontrado.\n");
        return 0;
    }
    if (!database_executar("BEGIN;")) {
        free(seed);
        return 0;
    }
    ok = database_executar(seed);
    free(seed);
    if (!ok) {
        database_executar("ROLLBACK;");
        printf("Falha ao popular dados de teste. Nenhuma alteracao foi gravada.\n");
        return 0;
    }
    if (!database_executar("COMMIT;")) {
        database_executar("ROLLBACK;");
        return 0;
    }
    printf("Banco populado com dados de teste com sucesso.\n");
    return 1;
}
