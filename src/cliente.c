#include "cliente.h"
#include "database.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>

static void cliente_listar_interno(const char *sql, sqlite3_stmt *stmt_pronto)
{
    sqlite3_stmt *stmt = stmt_pronto;
    int proprios = 0;

    if (!stmt) {
        if (!database_preparar(&stmt, sql)) {
            return;
        }
        proprios = 1;
    }

    printf("\n%-4s %-22s %-20s %-28s %-14s %-12s %s\n",
           "ID", "Nome", "Empresa", "Email", "Telefone", "Cidade", "Ativo");
    utils_separador();

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *nome = sqlite3_column_text(stmt, 1);
        const unsigned char *emp = sqlite3_column_text(stmt, 2);
        const unsigned char *email = sqlite3_column_text(stmt, 3);
        const unsigned char *fone = sqlite3_column_text(stmt, 4);
        const unsigned char *cid = sqlite3_column_text(stmt, 5);
        printf("%-4d %-22.22s %-20.20s %-28.28s %-14.14s %-12.12s %s\n",
               sqlite3_column_int(stmt, 0),
               nome ? (const char *)nome : "",
               emp ? (const char *)emp : "",
               email ? (const char *)email : "",
               fone ? (const char *)fone : "",
               cid ? (const char *)cid : "",
               sqlite3_column_int(stmt, 7) ? "sim" : "nao");
    }
    if (proprios) {
        sqlite3_finalize(stmt);
    }
}

void cliente_exibir_por_id(int id)
{
    sqlite3_stmt *stmt = NULL;
    const char *sql =
        "SELECT id, nome, empresa, email, telefone, cidade, estado, "
        "data_cadastro, ativo FROM clientes WHERE id = ?;";

    if (!database_preparar(&stmt, sql)) {
        return;
    }
    sqlite3_bind_int(stmt, 1, id);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        printf("Cliente %d nao encontrado.\n", id);
        sqlite3_finalize(stmt);
        return;
    }
    printf("ID:            %d\n", sqlite3_column_int(stmt, 0));
    printf("Nome:          %s\n", sqlite3_column_text(stmt, 1));
    printf("Empresa:       %s\n", sqlite3_column_text(stmt, 2) ? (const char *)sqlite3_column_text(stmt, 2) : "-");
    printf("E-mail:        %s\n", sqlite3_column_text(stmt, 3));
    printf("Telefone:      %s\n", sqlite3_column_text(stmt, 4) ? (const char *)sqlite3_column_text(stmt, 4) : "-");
    printf("Cidade:        %s\n", sqlite3_column_text(stmt, 5) ? (const char *)sqlite3_column_text(stmt, 5) : "-");
    printf("Estado:        %s\n", sqlite3_column_text(stmt, 6) ? (const char *)sqlite3_column_text(stmt, 6) : "-");
    printf("Cadastro:      %s\n", sqlite3_column_text(stmt, 7));
    printf("Ativo:         %s\n", sqlite3_column_int(stmt, 8) ? "sim" : "nao");
    sqlite3_finalize(stmt);
}

int cliente_existe_ativo(int id)
{
    sqlite3_stmt *stmt = NULL;
    int ok = 0;
    const char *sql = "SELECT ativo FROM clientes WHERE id = ?;";

    if (!database_preparar(&stmt, sql)) {
        return 0;
    }
    sqlite3_bind_int(stmt, 1, id);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        ok = sqlite3_column_int(stmt, 0) == 1;
    }
    sqlite3_finalize(stmt);
    return ok;
}

static void cliente_cadastrar(void)
{
    Cliente c;
    sqlite3_stmt *stmt = NULL;
    const char *sql =
        "INSERT INTO clientes (nome, empresa, email, telefone, cidade, estado, data_cadastro, ativo) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, 1);";

    memset(&c, 0, sizeof c);
    utils_titulo("Cadastrar cliente");
    if (!utils_ler_string("Nome: ", c.nome, sizeof c.nome, 1)) {
        return;
    }
    if (!utils_ler_string("Empresa (opcional): ", c.empresa, sizeof c.empresa, 0)) {
        return;
    }
    if (!utils_ler_string("E-mail: ", c.email, sizeof c.email, 1)) {
        return;
    }
    if (!utils_validar_email(c.email)) {
        printf("E-mail invalido. Informe um valor com '@' e '.' apos o dominio.\n");
        return;
    }
    if (!utils_ler_string("Telefone (opcional): ", c.telefone, sizeof c.telefone, 0)) {
        return;
    }
    if (!utils_ler_string("Cidade (opcional): ", c.cidade, sizeof c.cidade, 0)) {
        return;
    }
    if (!utils_ler_string("Estado UF (opcional): ", c.estado, sizeof c.estado, 0)) {
        return;
    }

    utils_data_hoje(c.data_cadastro, sizeof c.data_cadastro);
    if (!database_preparar(&stmt, sql)) {
        return;
    }
    sqlite3_bind_text(stmt, 1, c.nome, -1, SQLITE_TRANSIENT);
    if (utils_vazio(c.empresa)) {
        sqlite3_bind_null(stmt, 2);
    } else {
        sqlite3_bind_text(stmt, 2, c.empresa, -1, SQLITE_TRANSIENT);
    }
    sqlite3_bind_text(stmt, 3, c.email, -1, SQLITE_TRANSIENT);
    if (utils_vazio(c.telefone)) {
        sqlite3_bind_null(stmt, 4);
    } else {
        sqlite3_bind_text(stmt, 4, c.telefone, -1, SQLITE_TRANSIENT);
    }
    if (utils_vazio(c.cidade)) {
        sqlite3_bind_null(stmt, 5);
    } else {
        sqlite3_bind_text(stmt, 5, c.cidade, -1, SQLITE_TRANSIENT);
    }
    if (utils_vazio(c.estado)) {
        sqlite3_bind_null(stmt, 6);
    } else {
        sqlite3_bind_text(stmt, 6, c.estado, -1, SQLITE_TRANSIENT);
    }
    sqlite3_bind_text(stmt, 7, c.data_cadastro, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        if (sqlite3_extended_errcode(database_conexao()) == SQLITE_CONSTRAINT_UNIQUE) {
            printf("Ja existe um cliente com este e-mail.\n");
        } else {
            database_print_erro("Nao foi possivel cadastrar o cliente");
        }
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);
    printf("Cliente cadastrado com ID %lld.\n", database_ultimo_id());
}

static void cliente_listar(void)
{
    utils_titulo("Lista de clientes");
    cliente_listar_interno(
        "SELECT id, nome, empresa, email, telefone, cidade, estado, ativo "
        "FROM clientes ORDER BY id;",
        NULL);
}

static void cliente_buscar(void)
{
    int op;
    sqlite3_stmt *stmt = NULL;

    utils_titulo("Buscar cliente");
    printf("1 - Por ID\n2 - Por nome\n3 - Por e-mail\n");
    if (!utils_ler_int_faixa("Opcao: ", &op, 1, 3)) {
        return;
    }

    if (op == 1) {
        int id;
        if (!utils_ler_int("ID: ", &id)) {
            return;
        }
        cliente_exibir_por_id(id);
        return;
    }

    if (op == 2) {
        char nome[TAM_NOME];
        char like[TAM_NOME + 4];
        if (!utils_ler_string("Nome (parcial): ", nome, sizeof nome, 1)) {
            return;
        }
        snprintf(like, sizeof like, "%%%s%%", nome);
        if (!database_preparar(&stmt,
                               "SELECT id, nome, empresa, email, telefone, cidade, estado, ativo "
                               "FROM clientes WHERE nome LIKE ? ORDER BY nome;")) {
            return;
        }
        sqlite3_bind_text(stmt, 1, like, -1, SQLITE_TRANSIENT);
        cliente_listar_interno(NULL, stmt);
        sqlite3_finalize(stmt);
        return;
    }

    {
        char email[TAM_EMAIL];
        if (!utils_ler_string("E-mail: ", email, sizeof email, 1)) {
            return;
        }
        if (!database_preparar(&stmt,
                               "SELECT id, nome, empresa, email, telefone, cidade, estado, ativo "
                               "FROM clientes WHERE email = ?;")) {
            return;
        }
        sqlite3_bind_text(stmt, 1, email, -1, SQLITE_TRANSIENT);
        cliente_listar_interno(NULL, stmt);
        sqlite3_finalize(stmt);
    }
}

static void cliente_editar(void)
{
    int id;
    char nome[TAM_NOME], empresa[TAM_EMPRESA], email[TAM_EMAIL];
    char telefone[TAM_FONE], cidade[TAM_CIDADE], estado[TAM_ESTADO];
    sqlite3_stmt *stmt = NULL;
    const char *sql =
        "UPDATE clientes SET nome = ?, empresa = ?, email = ?, telefone = ?, "
        "cidade = ?, estado = ? WHERE id = ?;";

    utils_titulo("Editar cliente");
    if (!utils_ler_int("ID do cliente: ", &id)) {
        return;
    }
    cliente_exibir_por_id(id);
    printf("\nInforme os novos dados (campos obrigatorios nao podem ficar vazios).\n");

    if (!utils_ler_string("Nome: ", nome, sizeof nome, 1)) {
        return;
    }
    if (!utils_ler_string("Empresa (opcional): ", empresa, sizeof empresa, 0)) {
        return;
    }
    if (!utils_ler_string("E-mail: ", email, sizeof email, 1)) {
        return;
    }
    if (!utils_validar_email(email)) {
        printf("E-mail invalido.\n");
        return;
    }
    if (!utils_ler_string("Telefone (opcional): ", telefone, sizeof telefone, 0)) {
        return;
    }
    if (!utils_ler_string("Cidade (opcional): ", cidade, sizeof cidade, 0)) {
        return;
    }
    if (!utils_ler_string("Estado UF (opcional): ", estado, sizeof estado, 0)) {
        return;
    }
    if (!utils_confirmar("Confirmar alteracao")) {
        printf("Alteracao cancelada.\n");
        return;
    }

    if (!database_preparar(&stmt, sql)) {
        return;
    }
    sqlite3_bind_text(stmt, 1, nome, -1, SQLITE_TRANSIENT);
    if (utils_vazio(empresa)) sqlite3_bind_null(stmt, 2);
    else sqlite3_bind_text(stmt, 2, empresa, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, email, -1, SQLITE_TRANSIENT);
    if (utils_vazio(telefone)) sqlite3_bind_null(stmt, 4);
    else sqlite3_bind_text(stmt, 4, telefone, -1, SQLITE_TRANSIENT);
    if (utils_vazio(cidade)) sqlite3_bind_null(stmt, 5);
    else sqlite3_bind_text(stmt, 5, cidade, -1, SQLITE_TRANSIENT);
    if (utils_vazio(estado)) sqlite3_bind_null(stmt, 6);
    else sqlite3_bind_text(stmt, 6, estado, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 7, id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        if (sqlite3_extended_errcode(database_conexao()) == SQLITE_CONSTRAINT_UNIQUE) {
            printf("Ja existe outro cliente com este e-mail.\n");
        } else {
            database_print_erro("Falha ao atualizar cliente");
        }
    } else if (sqlite3_changes(database_conexao()) == 0) {
        printf("Cliente nao encontrado.\n");
    } else {
        printf("Cliente atualizado.\n");
    }
    sqlite3_finalize(stmt);
}

static void cliente_excluir(void)
{
    int id, rmas;
    sqlite3_stmt *stmt = NULL;

    utils_titulo("Excluir cliente (logico)");
    if (!utils_ler_int("ID do cliente: ", &id)) {
        return;
    }
    cliente_exibir_por_id(id);

    if (!database_preparar(&stmt, "SELECT COUNT(*) FROM rmas WHERE cliente_id = ?;")) {
        return;
    }
    sqlite3_bind_int(stmt, 1, id);
    rmas = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        rmas = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    if (rmas > 0) {
        printf("Este cliente possui %d RMA(s) vinculado(s).\n", rmas);
        printf("A exclusao fisica e impedida. A exclusao logica (inativar) continua disponivel.\n");
    }

    if (!utils_confirmar("Inativar este cliente")) {
        printf("Operacao cancelada.\n");
        return;
    }

    if (!database_preparar(&stmt, "UPDATE clientes SET ativo = 0 WHERE id = ?;")) {
        return;
    }
    sqlite3_bind_int(stmt, 1, id);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        database_print_erro("Falha ao inativar cliente");
    } else if (sqlite3_changes(database_conexao()) == 0) {
        printf("Cliente nao encontrado.\n");
    } else {
        printf("Cliente inativado (exclusao logica).\n");
    }
    sqlite3_finalize(stmt);
}

void cliente_menu(void)
{
    int op = -1;
    while (op != 0) {
        utils_limpar_tela();
        utils_titulo("Gerenciar clientes");
        printf("1 - Cadastrar cliente\n");
        printf("2 - Listar clientes\n");
        printf("3 - Buscar cliente\n");
        printf("4 - Editar cliente\n");
        printf("5 - Excluir (inativar) cliente\n");
        printf("0 - Voltar\n");
        if (!utils_ler_int("Opcao: ", &op)) {
            op = -1;
            continue;
        }
        switch (op) {
        case 1: cliente_cadastrar(); utils_pausar(); break;
        case 2: cliente_listar(); utils_pausar(); break;
        case 3: cliente_buscar(); utils_pausar(); break;
        case 4: cliente_editar(); utils_pausar(); break;
        case 5: cliente_excluir(); utils_pausar(); break;
        case 0: break;
        default: printf("Opcao invalida.\n"); utils_pausar(); break;
        }
    }
}
