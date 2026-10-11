#include "rma.h"
#include "cliente.h"
#include "database.h"
#include "estoque.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>

static int historico_inserir(int rma_id, const char *anterior, const char *novo, const char *descricao)
{
    sqlite3_stmt *stmt = NULL;
    char hoje[TAM_DATA];
    int ok = 0;

    utils_data_hoje(hoje, sizeof hoje);
    if (!database_preparar(&stmt,
                           "INSERT INTO historico_rma (rma_id, data_evento, status_anterior, status_novo, descricao) "
                           "VALUES (?, ?, ?, ?, ?);")) {
        return 0;
    }
    sqlite3_bind_int(stmt, 1, rma_id);
    sqlite3_bind_text(stmt, 2, hoje, -1, SQLITE_TRANSIENT);
    if (anterior) {
        sqlite3_bind_text(stmt, 3, anterior, -1, SQLITE_TRANSIENT);
    } else {
        sqlite3_bind_null(stmt, 3);
    }
    sqlite3_bind_text(stmt, 4, novo, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, descricao, -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_DONE) {
        ok = 1;
    } else {
        database_print_erro("Falha ao gravar historico do RMA");
    }
    sqlite3_finalize(stmt);
    return ok;
}

static int rma_tem_aberto_para_serie(int serie_id)
{
    sqlite3_stmt *stmt = NULL;
    int n = 0;
    const char *sql =
        "SELECT COUNT(*) FROM rmas WHERE estoque_serie_id = ? "
        "AND status NOT IN ('CONCLUIDO', 'REJEITADO', 'CANCELADO');";

    if (!database_preparar(&stmt, sql)) return 1;
    sqlite3_bind_int(stmt, 1, serie_id);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        n = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return n > 0;
}

static int rma_carregar(int id, int *cliente_id, int *serie_id, char *status, size_t tam)
{
    sqlite3_stmt *stmt = NULL;
    int ok = 0;

    if (!database_preparar(&stmt, "SELECT cliente_id, estoque_serie_id, status FROM rmas WHERE id = ?;")) {
        return 0;
    }
    sqlite3_bind_int(stmt, 1, id);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        if (cliente_id) *cliente_id = sqlite3_column_int(stmt, 0);
        if (serie_id) *serie_id = sqlite3_column_int(stmt, 1);
        if (status && tam > 0) {
            snprintf(status, tam, "%s", (const char *)sqlite3_column_text(stmt, 2));
        }
        ok = 1;
    }
    sqlite3_finalize(stmt);
    return ok;
}

static void rma_listar_sql(sqlite3_stmt *stmt)
{
    printf("\n%-4s %-10s %-10s %-8s %-22s %-20s %s\n",
           "ID", "Abertura", "Status", "Prio", "Cliente", "Produto", "Serial");
    utils_separador();
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("%-4d %-10.10s %-10.10s %-8.8s %-22.22s %-20.20s %s\n",
               sqlite3_column_int(stmt, 0),
               sqlite3_column_text(stmt, 1),
               sqlite3_column_text(stmt, 2),
               sqlite3_column_text(stmt, 3),
               sqlite3_column_text(stmt, 4),
               sqlite3_column_text(stmt, 5),
               sqlite3_column_text(stmt, 6));
    }
}

static const char *SQL_LISTA_RMA =
    "SELECT r.id, r.data_abertura, r.status, r.prioridade, c.nome, p.nome, s.numero_serie "
    "FROM rmas r "
    "JOIN clientes c ON c.id = r.cliente_id "
    "JOIN estoque_series s ON s.id = r.estoque_serie_id "
    "JOIN produtos p ON p.id = s.produto_id ";

static void rma_listar_todos(void)
{
    sqlite3_stmt *stmt = NULL;
    char sql[1024];

    snprintf(sql, sizeof sql, "%s ORDER BY r.id;", SQL_LISTA_RMA);
    utils_titulo("Lista de RMAs");
    if (!database_preparar(&stmt, sql)) return;
    rma_listar_sql(stmt);
    sqlite3_finalize(stmt);
}

static void rma_consultar_detalhes(void)
{
    int id;
    sqlite3_stmt *stmt = NULL;

    utils_titulo("Detalhes do RMA");
    if (!utils_ler_int("ID do RMA: ", &id)) return;

    if (!database_preparar(&stmt,
                           "SELECT r.id, r.data_abertura, r.data_conclusao, r.status, r.prioridade, "
                           "r.descricao_problema, r.observacoes, "
                           "c.id, c.nome, c.empresa, c.email, "
                           "s.id, s.numero_serie, s.status, "
                           "p.id, p.nome, p.modelo, p.meses_garantia, "
                           "cp.data_compra "
                           "FROM rmas r "
                           "JOIN clientes c ON c.id = r.cliente_id "
                           "JOIN estoque_series s ON s.id = r.estoque_serie_id "
                           "JOIN produtos p ON p.id = s.produto_id "
                           "LEFT JOIN clientes_produtos cp ON cp.estoque_serie_id = s.id "
                           "WHERE r.id = ?;")) {
        return;
    }
    sqlite3_bind_int(stmt, 1, id);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        printf("RMA nao encontrado.\n");
        sqlite3_finalize(stmt);
        return;
    }

    printf("RMA ID:            %d\n", sqlite3_column_int(stmt, 0));
    printf("Abertura:          %s\n", sqlite3_column_text(stmt, 1));
    printf("Conclusao:         %s\n", sqlite3_column_text(stmt, 2) ? (const char *)sqlite3_column_text(stmt, 2) : "-");
    printf("Status:            %s\n", sqlite3_column_text(stmt, 3));
    printf("Prioridade:        %s\n", sqlite3_column_text(stmt, 4));
    printf("Problema:          %s\n", sqlite3_column_text(stmt, 5));
    printf("Observacoes:       %s\n", sqlite3_column_text(stmt, 6) ? (const char *)sqlite3_column_text(stmt, 6) : "-");
    printf("\nCliente ID:        %d\n", sqlite3_column_int(stmt, 7));
    printf("Cliente:           %s\n", sqlite3_column_text(stmt, 8));
    printf("Empresa:           %s\n", sqlite3_column_text(stmt, 9) ? (const char *)sqlite3_column_text(stmt, 9) : "-");
    printf("E-mail:            %s\n", sqlite3_column_text(stmt, 10));
    printf("\nSerie ID:          %d\n", sqlite3_column_int(stmt, 11));
    printf("Numero de serie:   %s\n", sqlite3_column_text(stmt, 12));
    printf("Status da serie:   %s\n", sqlite3_column_text(stmt, 13));
    printf("Produto:           %s (%s)\n", sqlite3_column_text(stmt, 15), sqlite3_column_text(stmt, 16));
    printf("Garantia (meses):  %d\n", sqlite3_column_int(stmt, 17));
    printf("Data da compra:    %s\n", sqlite3_column_text(stmt, 18) ? (const char *)sqlite3_column_text(stmt, 18) : "-");
    sqlite3_finalize(stmt);

    printf("\n--- Historico ---\n");
    if (!database_preparar(&stmt,
                           "SELECT data_evento, status_anterior, status_novo, descricao "
                           "FROM historico_rma WHERE rma_id = ? ORDER BY id;")) {
        return;
    }
    sqlite3_bind_int(stmt, 1, id);
    printf("%-12s %-16s %-16s %s\n", "Data", "Anterior", "Novo", "Descricao");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("%-12.12s %-16.16s %-16.16s %s\n",
               sqlite3_column_text(stmt, 0),
               sqlite3_column_text(stmt, 1) ? (const char *)sqlite3_column_text(stmt, 1) : "-",
               sqlite3_column_text(stmt, 2),
               sqlite3_column_text(stmt, 3));
    }
    sqlite3_finalize(stmt);

    printf("\n--- Pecas utilizadas ---\n");
    if (!database_preparar(&stmt,
                           "SELECT pu.id, p.nome, pu.quantidade, pu.data_utilizacao "
                           "FROM pecas_utilizadas pu JOIN pecas p ON p.id = pu.peca_id "
                           "WHERE pu.rma_id = ? ORDER BY pu.id;")) {
        return;
    }
    sqlite3_bind_int(stmt, 1, id);
    {
        int alguma = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            alguma = 1;
            printf("  - %s  qtd=%d  em %s\n",
                   sqlite3_column_text(stmt, 1),
                   sqlite3_column_int(stmt, 2),
                   sqlite3_column_text(stmt, 3));
        }
        if (!alguma) {
            printf("(nenhuma peca registrada)\n");
        }
    }
    sqlite3_finalize(stmt);
}

static void rma_abrir(void)
{
    int cliente_id, serie_id, produto_id, dono_id, meses = 0;
    char serial[TAM_SERIAL], status[TAM_STATUS], data_compra[TAM_DATA];
    char descricao[TAM_TEXTO], prioridade[TAM_STATUS], obs[TAM_TEXTO];
    char hoje[TAM_DATA], vencimento[TAM_DATA];
    int fora = 0;
    sqlite3_stmt *stmt = NULL;

    utils_titulo("Abrir RMA");
    if (!utils_ler_int("ID do cliente: ", &cliente_id)) return;
    if (!cliente_existe_ativo(cliente_id)) {
        printf("Cliente inexistente ou inativo.\n");
        return;
    }
    if (!utils_ler_string("Numero de serie: ", serial, sizeof serial, 1)) return;
    if (!estoque_serie_por_numero(serial, &serie_id, &produto_id, status, sizeof status)) {
        printf("Numero de serie nao encontrado.\n");
        return;
    }
    if (strcmp(status, "VENDIDO") != 0) {
        printf("Nao e possivel abrir RMA: o numero de serie precisa estar VENDIDO (atual: %s).\n", status);
        return;
    }
    if (!estoque_cliente_da_serie(serie_id, &dono_id, data_compra, sizeof data_compra)) {
        printf("Este numero de serie nao esta vinculado a nenhum cliente.\n");
        return;
    }
    if (dono_id != cliente_id) {
        printf("O numero de serie pertence ao cliente %d, nao ao cliente %d.\n", dono_id, cliente_id);
        return;
    }
    if (rma_tem_aberto_para_serie(serie_id)) {
        printf("Ja existe um RMA em andamento para este numero de serie.\n");
        return;
    }

    if (!database_preparar(&stmt, "SELECT meses_garantia FROM produtos WHERE id = ?;")) return;
    sqlite3_bind_int(stmt, 1, produto_id);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        meses = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    utils_data_add_meses(data_compra, meses, vencimento, sizeof vencimento);
    if (!utils_dentro_garantia(data_compra, meses)) {
        fora = 1;
        printf("\n*** AVISO: produto FORA DE GARANTIA. ***\n");
        printf("Compra: %s | Garantia: %d meses | Vencimento: %s\n", data_compra, meses, vencimento);
        if (!utils_confirmar("Abrir RMA mesmo assim")) {
            printf("Abertura cancelada.\n");
            return;
        }
    } else {
        printf("Produto DENTRO da garantia (compra %s, vencimento %s).\n", data_compra, vencimento);
    }

    if (!utils_ler_string("Descricao do problema: ", descricao, sizeof descricao, 1)) return;
    utils_menu_prioridade();
    if (!utils_ler_string("Prioridade [MEDIA]: ", prioridade, sizeof prioridade, 0)) return;
    if (utils_vazio(prioridade)) {
        snprintf(prioridade, sizeof prioridade, "MEDIA");
    }
    if (!utils_prioridade_valida(prioridade)) {
        printf("Prioridade invalida.\n");
        return;
    }

    obs[0] = '\0';
    if (fora) {
        snprintf(obs, sizeof obs, "FORA DE GARANTIA");
    }

    utils_data_hoje(hoje, sizeof hoje);
    database_executar("BEGIN;");

    if (!database_preparar(&stmt,
                           "INSERT INTO rmas (cliente_id, estoque_serie_id, data_abertura, descricao_problema, "
                           "status, prioridade, data_conclusao, observacoes) "
                           "VALUES (?, ?, ?, ?, 'ABERTO', ?, NULL, ?);")) {
        database_executar("ROLLBACK;");
        return;
    }
    sqlite3_bind_int(stmt, 1, cliente_id);
    sqlite3_bind_int(stmt, 2, serie_id);
    sqlite3_bind_text(stmt, 3, hoje, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, descricao, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, prioridade, -1, SQLITE_TRANSIENT);
    if (obs[0]) sqlite3_bind_text(stmt, 6, obs, -1, SQLITE_TRANSIENT);
    else sqlite3_bind_null(stmt, 6);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        database_print_erro("Falha ao abrir RMA");
        sqlite3_finalize(stmt);
        database_executar("ROLLBACK;");
        return;
    }
    sqlite3_finalize(stmt);

    {
        int rma_id = (int)database_ultimo_id();
        if (!estoque_atualizar_status_serie(serie_id, "EM_RMA")) {
            database_executar("ROLLBACK;");
            return;
        }
        if (!historico_inserir(rma_id, NULL, "ABERTO",
                               fora ? "RMA aberto (FORA DE GARANTIA)." : "RMA aberto.")) {
            database_executar("ROLLBACK;");
            return;
        }
        database_executar("COMMIT;");
        printf("RMA %d aberto. Numero de serie alterado para EM_RMA.\n", rma_id);
    }
}

static int rma_registrar_pecas(int rma_id)
{
    char hoje[TAM_DATA];

    utils_data_hoje(hoje, sizeof hoje);
    peca_alertar_minimos();
    printf("Informe as pecas utilizadas. ID 0 encerra.\n");

    for (;;) {
        int peca_id = 0, qtd = 0;
        sqlite3_stmt *stmt = NULL;

        if (!utils_ler_int("ID da peca (0 para terminar): ", &peca_id)) {
            continue;
        }
        if (peca_id == 0) {
            break;
        }
        if (!utils_ler_int("Quantidade: ", &qtd) || qtd <= 0) {
            printf("Quantidade invalida.\n");
            continue;
        }
        if (!peca_baixar_estoque(peca_id, qtd)) {
            continue;
        }
        if (!database_preparar(&stmt,
                               "INSERT INTO pecas_utilizadas (rma_id, peca_id, quantidade, data_utilizacao) "
                               "VALUES (?, ?, ?, ?);")) {
            return 0;
        }
        sqlite3_bind_int(stmt, 1, rma_id);
        sqlite3_bind_int(stmt, 2, peca_id);
        sqlite3_bind_int(stmt, 3, qtd);
        sqlite3_bind_text(stmt, 4, hoje, -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            database_print_erro("Falha ao registrar peca utilizada");
            sqlite3_finalize(stmt);
            return 0;
        }
        sqlite3_finalize(stmt);
        printf("Peca %d registrada (%d unidade(s)).\n", peca_id, qtd);
    }
    return 1;
}

static void rma_alterar_status(void)
{
    int id, cliente_id, serie_id;
    char atual[TAM_STATUS], novo[TAM_STATUS], desc[TAM_TEXTO], hoje[TAM_DATA];
    sqlite3_stmt *stmt = NULL;

    utils_titulo("Alterar status do RMA");
    if (!utils_ler_int("ID do RMA: ", &id)) return;
    if (!rma_carregar(id, &cliente_id, &serie_id, atual, sizeof atual)) {
        printf("RMA nao encontrado.\n");
        return;
    }
    printf("Status atual: %s\n", atual);
    if (!utils_rma_status_aberto(atual) && strcmp(atual, "CONCLUIDO") == 0) {
        printf("Este RMA ja esta encerrado (%s).\n", atual);
        if (!utils_confirmar("Alterar mesmo assim")) return;
    }

    utils_menu_status_rma();
    if (!utils_ler_string("Novo status: ", novo, sizeof novo, 1)) return;
    if (!utils_status_rma_valido(novo)) {
        printf("Status invalido. Informe um status final valido da lista.\n");
        return;
    }
    if (strcmp(atual, novo) == 0) {
        printf("O RMA ja esta neste status.\n");
        return;
    }
    if (strcmp(novo, "CONCLUIDO") == 0 || strcmp(novo, "REJEITADO") == 0 ||
        strcmp(novo, "CANCELADO") == 0) {
        if (!utils_confirmar("Confirmar mudanca de status critica")) {
            printf("Operacao cancelada.\n");
            return;
        }
    }

    if (!utils_ler_string("Descricao do evento no historico: ", desc, sizeof desc, 1)) return;
    utils_data_hoje(hoje, sizeof hoje);

    database_executar("BEGIN;");

    if (strcmp(novo, "CONCLUIDO") == 0) {
        if (!database_preparar(&stmt,
                               "UPDATE rmas SET status = ?, data_conclusao = ? WHERE id = ?;")) {
            database_executar("ROLLBACK;");
            return;
        }
        sqlite3_bind_text(stmt, 1, novo, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, hoje, -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 3, id);
    } else {
        if (!database_preparar(&stmt, "UPDATE rmas SET status = ? WHERE id = ?;")) {
            database_executar("ROLLBACK;");
            return;
        }
        sqlite3_bind_text(stmt, 1, novo, -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, id);
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        database_print_erro("Falha ao atualizar RMA");
        sqlite3_finalize(stmt);
        database_executar("ROLLBACK;");
        return;
    }
    sqlite3_finalize(stmt);

    if (!historico_inserir(id, atual, novo, desc)) {
        database_executar("ROLLBACK;");
        return;
    }

    if (strcmp(novo, "CONCLUIDO") == 0) {
        if (utils_confirmar("Houve substituicao de pecas")) {
            if (!rma_registrar_pecas(id)) {
                database_executar("ROLLBACK;");
                return;
            }
        }
        if (utils_confirmar("A unidade foi substituida (serial vai para SUBSTITUIDO)")) {
            if (!estoque_atualizar_status_serie(serie_id, "SUBSTITUIDO")) {
                database_executar("ROLLBACK;");
                return;
            }
        } else {
            if (!estoque_atualizar_status_serie(serie_id, "VENDIDO")) {
                database_executar("ROLLBACK;");
                return;
            }
        }
    } else if (strcmp(novo, "REJEITADO") == 0 || strcmp(novo, "CANCELADO") == 0) {
        if (!estoque_atualizar_status_serie(serie_id, "VENDIDO")) {
            database_executar("ROLLBACK;");
            return;
        }
        printf("Numero de serie retornado para VENDIDO.\n");
    }

    database_executar("COMMIT;");
    printf("Status do RMA %d alterado de %s para %s.\n", id, atual, novo);
}

static void rma_filtrar(void)
{
    int op;
    sqlite3_stmt *stmt = NULL;
    char sql[1200];

    utils_titulo("Listar RMAs com filtro");
    printf("1 - Por status\n2 - Por cliente\n3 - Por produto (nome)\n4 - Por periodo\n");
    if (!utils_ler_int_faixa("Opcao: ", &op, 1, 4)) return;

    if (op == 1) {
        char status[TAM_STATUS];
        utils_menu_status_rma();
        if (!utils_ler_string("Status: ", status, sizeof status, 1)) return;
        if (!utils_status_rma_valido(status)) {
            printf("Status invalido.\n");
            return;
        }
        snprintf(sql, sizeof sql, "%s WHERE r.status = ? ORDER BY r.id;", SQL_LISTA_RMA);
        if (!database_preparar(&stmt, sql)) return;
        sqlite3_bind_text(stmt, 1, status, -1, SQLITE_TRANSIENT);
    } else if (op == 2) {
        int cliente_id;
        if (!utils_ler_int("ID do cliente: ", &cliente_id)) return;
        snprintf(sql, sizeof sql, "%s WHERE r.cliente_id = ? ORDER BY r.id;", SQL_LISTA_RMA);
        if (!database_preparar(&stmt, sql)) return;
        sqlite3_bind_int(stmt, 1, cliente_id);
    } else if (op == 3) {
        char nome[TAM_NOME], like[TAM_NOME + 4];
        if (!utils_ler_string("Nome do produto (parcial): ", nome, sizeof nome, 1)) return;
        snprintf(like, sizeof like, "%%%s%%", nome);
        snprintf(sql, sizeof sql, "%s WHERE p.nome LIKE ? ORDER BY r.id;", SQL_LISTA_RMA);
        if (!database_preparar(&stmt, sql)) return;
        sqlite3_bind_text(stmt, 1, like, -1, SQLITE_TRANSIENT);
    } else {
        char ini[TAM_DATA], fim[TAM_DATA];
        if (!utils_ler_string("Data inicial (AAAA-MM-DD): ", ini, sizeof ini, 1) ||
            !utils_validar_data(ini)) {
            printf("Data inicial invalida.\n");
            return;
        }
        if (!utils_ler_string("Data final (AAAA-MM-DD): ", fim, sizeof fim, 1) ||
            !utils_validar_data(fim)) {
            printf("Data final invalida.\n");
            return;
        }
        snprintf(sql, sizeof sql, "%s WHERE r.data_abertura BETWEEN ? AND ? ORDER BY r.data_abertura;", SQL_LISTA_RMA);
        if (!database_preparar(&stmt, sql)) return;
        sqlite3_bind_text(stmt, 1, ini, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, fim, -1, SQLITE_TRANSIENT);
    }

    rma_listar_sql(stmt);
    sqlite3_finalize(stmt);
}

void rma_menu(void)
{
    int op = -1;
    while (op != 0) {
        utils_limpar_tela();
        utils_titulo("Gerenciar RMAs");
        printf("1 - Abrir RMA\n");
        printf("2 - Alterar status\n");
        printf("3 - Listar todos\n");
        printf("4 - Listar com filtro (status/cliente/produto/periodo)\n");
        printf("5 - Consultar detalhes completos\n");
        printf("0 - Voltar\n");
        if (!utils_ler_int("Opcao: ", &op)) { op = -1; continue; }
        switch (op) {
        case 1: rma_abrir(); utils_pausar(); break;
        case 2: rma_alterar_status(); utils_pausar(); break;
        case 3: rma_listar_todos(); utils_pausar(); break;
        case 4: rma_filtrar(); utils_pausar(); break;
        case 5: rma_consultar_detalhes(); utils_pausar(); break;
        case 0: break;
        default: printf("Opcao invalida.\n"); utils_pausar(); break;
        }
    }
}
