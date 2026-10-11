#ifndef DATABASE_H
#define DATABASE_H

#include "sqlite3.h"

int          database_abrir(const char *caminho);
void         database_fechar(void);
sqlite3     *database_conexao(void);
int          database_inicializar(void);
int          database_popular_teste(void);
int          database_preparar(sqlite3_stmt **stmt, const char *sql);
void         database_print_erro(const char *contexto);
long long    database_ultimo_id(void);
int          database_imprimir_consulta(const char *sql);
int          database_executar(const char *sql);
int          database_contar(const char *sql);

#endif
